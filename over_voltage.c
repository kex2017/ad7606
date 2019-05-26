#include <stdint.h>
#include <string.h>

#include "log.h"
#include "board.h"
#include "periph/rtt.h"
#include "thread.h"
#include "x_delay.h"

#include "data_storage.h"
#include "over_voltage.h"
#include "daq.h"
#include "cable_ground_threads.h"
#include "upgrade_from_sd_card.h"

enum{
    FLAG_ON = 1,
    FLAG_OFF = -1
};
#define OVER_VOLTAGE_FLAG_ON 0x73
#define OVER_VOLTAGE_FLAG_OFF 0X37

uint8_t over_voltage_event_flag = OVER_VOLTAGE_FLAG_OFF;
uint32_t over_voltage_event_timestamp = 0;
over_voltage_event_info_t channel_over_voltage_event_info[MAX_OVER_VOLTAGE_CHANNEL_COUNT] = { {0} };

static uint32_t g_over_voltage_curve_data_len[MAX_OVER_VOLTAGE_CHANNEL_COUNT] = { 0 };
void set_over_voltage_curve_data_len(uint8_t channel, uint32_t length)
{
    g_over_voltage_curve_data_len[channel] = length;
}

uint32_t get_over_voltage_curve_data_len(uint8_t channel)
{
    return g_over_voltage_curve_data_len[channel];
}
/************************************************************
 * 过电压事件发生报告标志处理函数
 * **********************************************************/
void set_channel_over_voltage_happened_flag(uint8_t channel)
{
    channel_over_voltage_event_info[channel].happened_flag = FLAG_ON;
}

uint16_t get_channel_over_voltage_happened_flag(uint8_t channel)
{
    return channel_over_voltage_event_info[channel].happened_flag;
}

void clear_channel_over_voltage_happened_flag(uint8_t channel)
{
    channel_over_voltage_event_info[channel].happened_flag = FLAG_OFF;
}

/*************************************************************
 * 过电压事件对应通道的发生时间处理函数
 * ***********************************************************/
void set_channel_over_voltage_happened_timestamp(uint8_t channel, uint32_t timestamp)
{
    channel_over_voltage_event_info[channel].timestamp = timestamp;
}

uint32_t get_channel_over_voltage_happened_timestamp(uint8_t channel)
{
    return channel_over_voltage_event_info[channel].timestamp;
}

/*************************************************************
 * 过电压事件对应通道的波形最大值处理函数
 * ***********************************************************/
void set_channel_over_voltage_max_value(uint8_t channel, uint16_t max_value)
{
    channel_over_voltage_event_info[channel].max_value = max_value;
}

uint16_t get_channel_over_voltage_max_value(uint8_t channel)
{
    return channel_over_voltage_event_info[channel].max_value;
}

/*************************************************************
 * 过电压事件对应通道的波形的k系数处理函数
 * ***********************************************************/
void set_channel_over_voltage_k_coefficient(uint8_t channel, float k)
{
    channel_over_voltage_event_info[channel].k = k;
}

float get_channel_over_voltage_k_coefficient(uint8_t channel)
{
    return channel_over_voltage_event_info[channel].k;
}

/*************************************************************
 * 获取过电压事件对应通道每秒发生的过电压次数
 * ***********************************************************/
void set_channel_over_voltage_event_sn_info(uint8_t channel, uint16_t sn)
{
    channel_over_voltage_event_info[channel].sn = sn;
}

uint16_t get_channel_over_voltage_event_sn_info(uint8_t channel)
{
    return channel_over_voltage_event_info[channel].sn;
}

/*************************************************************
 * 过电压事件中断发生时间处理函数
 * ***********************************************************/
void set_over_voltage_event_timestamp(uint32_t timestamp)
{
    over_voltage_event_timestamp = timestamp;
}

uint32_t get_over_voltage_event_timestamp(void)
{
    return over_voltage_event_timestamp;
}

void clear_over_voltage_event_timestamp(void)
{
    over_voltage_event_timestamp = 0;
}

/*************************************************************
 * 过电压事件中断标志处理函数
 * ***********************************************************/
void set_over_voltage_event_flag(void)
{
    over_voltage_event_flag = OVER_VOLTAGE_FLAG_ON;
}

uint8_t get_over_voltage_event_flag(void)
{
    return over_voltage_event_flag;
}

void clear_over_voltage_event_flag(void)
{
    over_voltage_event_flag = OVER_VOLTAGE_FLAG_OFF;
}

/*************************************************************
 * 过电压事件信息维护函数
 * ***********************************************************/
void ov_event_info_channels_init(void)
{
    memset(channel_over_voltage_event_info, 0x0, sizeof(channel_over_voltage_event_info));
}

over_voltage_event_info_t *get_channel_over_voltage_event_info(uint8_t channel)
{
    return &channel_over_voltage_event_info[channel];
}

void update_channel_over_voltage_event_info(uint8_t channel)
{
    uint16_t sn = 0;
    int max_value = 0;
    if (get_channel_over_voltage_happened_timestamp(channel) != get_over_voltage_event_timestamp()) {
        set_channel_over_voltage_happened_timestamp(channel, get_over_voltage_event_timestamp());
        set_channel_over_voltage_event_sn_info(channel, sn);
        max_value = daq_spi_curve_max_get(channel);
        set_channel_over_voltage_max_value(channel, max_value > 2048 ? max_value - 2048 : max_value);
        set_channel_over_voltage_k_coefficient(channel, max_value > 2048 ? 1 : -1);
        set_channel_over_voltage_happened_flag(channel);
        return;
    }
}

#define MAX_FPGA_DATA_LEN (20480 + 4096) //24k
static uint8_t over_voltage_event_buffer[MAX_FPGA_DATA_LEN] __attribute__((section(".big_data")));
void over_voltage_process_one_event(uint32_t channels, uint32_t timestamp)
{
    uint8_t index = 0;
    uint32_t length = 0;

    for (index = 0; index < MAX_OVER_VOLTAGE_CHANNEL_COUNT; index++) {
        if (channels & (0x01 << index)) {
            acquire_spi_ops_mtx();
            if (0 < daq_spi_sample_done_check(index)) {
                set_over_voltage_event_timestamp(timestamp);
                if ((length = daq_spi_sample_len_get(index)) > MAX_FPGA_DATA_LEN) {
                    LOG_WARN("Get over voltage curve data length:%ld > MAX_FPGA_DATA_LEN, ignore it.", length);
                    continue;
                }
                set_over_voltage_curve_data_len(index, length);
                daq_spi_sample_data_read(index, over_voltage_event_buffer, 0, length);
                LOG_INFO("over voltage of channel:%d curve data length:%d ", index, length);
                update_channel_over_voltage_event_info(index);

                //FIXME: need to satisfy one channel 4 event;;;
                set_over_voltage_curve_data(index, 0, over_voltage_event_buffer, length);
                if (get_save_curve_data_to_sd_flag()) {
                    save_curve_data_to_sd(index, over_voltage_event_buffer, length);
                }
            }
            else
                LOG_WARN("spi fpga not sample done yet");
            acquire_spi_ops_mtx();
        }

    }

    //FIXME: parameter of channel is ignored in this function implementation
    acquire_spi_ops_mtx();
    daq_spi_ov_flag_reset(0);
    acquire_spi_ops_mtx();
}

int over_voltage_get_event_happened_channels(uint8_t *channels, uint8_t *count)
{
   uint8_t channel = 0;
   uint16_t happened_channel_count = 0;
   uint16_t number = 0;

   for (channel = 0; channel < MAX_OVER_VOLTAGE_CHANNEL_COUNT; channel++) {
      if (FLAG_ON == get_channel_over_voltage_happened_flag(channel)) {
         channels[number] = channel;
         happened_channel_count++;
         number++;
      }
   }

   *count = happened_channel_count;

   return 0;
}

int over_voltage_get_event_info(uint8_t channel, over_voltage_event_info_t *event)
{
   event->timestamp = channel_over_voltage_event_info[channel].timestamp;
   event->sn = channel_over_voltage_event_info[channel].sn;
   event->k = channel_over_voltage_event_info[channel].k;
   event->max_value = channel_over_voltage_event_info[channel].max_value;
   return 0;
}

int over_voltage_set_threshold(uint8_t channel, uint16_t threshold)
{
   daq_spi_channel_threshold_set(channel,threshold);
   return 0;
}

int over_voltage_set_changerate(uint8_t channel, uint16_t changerate)
{
   daq_spi_channel_changerate_set(channel,changerate);
   return 0;
}

uint16_t over_voltage_get_curve_data_len(uint8_t channel,  uint32_t timestamp, uint32_t *length)
{
    uint32_t ret = 0;
    if (get_channel_over_voltage_happened_timestamp(channel) < timestamp) {
        LOG_WARN("No Over Voltage Curve yet: Request future timestamp:%d ", timestamp);
        return -1;
    }

    *length = get_over_voltage_curve_data_len(channel);

    return ret;
}

int over_voltage_get_curve_data(uint8_t channel,  uint32_t timestamp, uint16_t sn, uint16_t cur_pkt_num, uint16_t* total_pkt_count, uint8_t *data, uint16_t pkt_size)
{
   (void)sn;
   uint32_t addr;
   uint32_t ret;

   //TODO: more accurate
   if (get_channel_over_voltage_happened_timestamp(channel) < timestamp) {
       LOG_WARN("No Over Voltage Curve yet: Request future timestamp:%d ", timestamp);
       return -1;
   }

   *total_pkt_count = get_over_voltage_curve_data_len(channel)/pkt_size;
   addr = get_over_voltage_start_addr(channel, sn);
   addr = addr + cur_pkt_num * pkt_size;
   ret = get_over_voltage_curve_data(data, addr, pkt_size);

   return ret;
}
