#include "over_current.h"

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "board.h"
#include "periph/rtt.h"
#include "thread.h"
#include "x_delay.h"
#include "periph/gpio.h"
#include "daq.h"

enum{
    FLAG_ON = 1,
    FLAG_OFF = -1
};
#define OVER_CURRENT_FLAG_ON 0x73
#define OVER_CURRENT_FLAG_OFF 0X37

/**
 * @brief   PID of the over current thread
 */
kernel_pid_t over_current_pid = KERNEL_PID_UNDEF;

#define  OVER_CURRENT_EVENT_PIN  GPIO_PIN(PORT_E,2)

void OVER_CURRENT_EXIT2_IRQHandler(void *arg);
void over_current_event_pins_init(void)
{
    gpio_clear(OVER_CURRENT_EVENT_PIN);
    gpio_init_int(OVER_CURRENT_EVENT_PIN, GPIO_IN, GPIO_RISING, OVER_CURRENT_EXIT2_IRQHandler, NULL);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    NVIC_DisableIRQ(EXTI2_IRQn);
}


void enable_over_current_irq(void)
{
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void disable_over_current_irq(void)
{
    NVIC_DisableIRQ(EXTI2_IRQn);
}

uint8_t over_current_event_flag = OVER_CURRENT_FLAG_OFF;
uint32_t over_current_event_timestamp = 0;
over_current_event_info_t channel_over_current_event_info[MAX_OVER_CURRENT_CHANNEL_COUNT] = { {0} };

static uint32_t g_over_current_curve_data_len[MAX_OVER_CURRENT_CHANNEL_COUNT] = { 0 };
void set_over_current_curve_data_len(uint8_t channel, uint32_t length)
{
    g_over_current_curve_data_len[channel] = length;
}

uint32_t get_over_current_curve_data_len(uint8_t channel)
{
    return g_over_current_curve_data_len[channel];
}
/************************************************************
 * 过电流事件发生报告标志处理函数
 * **********************************************************/
void set_channel_over_current_happened_flag(uint8_t channel)
{
    channel_over_current_event_info[channel].happened_flag = FLAG_ON;
}

uint16_t get_channel_over_current_happened_flag(uint8_t channel)
{
    return channel_over_current_event_info[channel].happened_flag;
}

void clear_channel_over_current_happened_flag(uint8_t channel)
{
    channel_over_current_event_info[channel].happened_flag = FLAG_OFF;
}
/*************************************************************
 * 过电流事件对应通道的发生时间处理函数
 * ***********************************************************/
void set_channel_over_current_happened_timestamp(uint8_t channel, uint32_t timestamp)
{
    channel_over_current_event_info[channel].timestamp = timestamp;
}

uint32_t get_channel_over_current_happened_timestamp(uint8_t channel)
{
    return channel_over_current_event_info[channel].timestamp;
}

/*************************************************************
 * 过电流事件对应通道的波形最大值处理函数
 * ***********************************************************/
void set_channel_over_current_max_value(uint8_t channel, uint16_t max_value)
{
    channel_over_current_event_info[channel].max_value = max_value;
}

uint16_t get_channel_over_current_max_value(uint8_t channel)
{
    return channel_over_current_event_info[channel].max_value;
}

/*************************************************************
 * 过电流事件对应通道的波形平均值处理函数
 * ***********************************************************/
void set_channel_over_current_avr_value(uint8_t channel, uint16_t avr_value)
{
    channel_over_current_event_info[channel].avr_value = avr_value;
}

uint16_t get_channel_over_current_avr_value(uint8_t channel)
{
    return channel_over_current_event_info[channel].avr_value;
}

/*************************************************************
 * 过电流事件对应通道的波形的k系数处理函数
 * ***********************************************************/
void set_channel_over_current_k_coefficient(uint8_t channel, float k)
{
    channel_over_current_event_info[channel].k = k;
}

float get_channel_over_current_k_coefficient(uint8_t channel)
{
    return channel_over_current_event_info[channel].k;
}

/*************************************************************
 * 获取过电流事件对应通道每秒发生的过电流次数
 * ***********************************************************/
void set_channel_over_current_event_sn_info(uint8_t channel, uint16_t sn)
{
    channel_over_current_event_info[channel].sn = sn;
}

uint16_t get_channel_over_current_event_sn_info(uint8_t channel)
{
    return channel_over_current_event_info[channel].sn;
}

/*************************************************************
 * 过电流事件中断标志处理函数
 * ***********************************************************/
void set_over_current_event_flag(void)
{
    over_current_event_flag = OVER_CURRENT_FLAG_ON;
}

uint8_t get_over_current_event_flag(void)
{
    return over_current_event_flag;
}

void clear_over_current_event_flag(void)
{
    over_current_event_flag = OVER_CURRENT_FLAG_OFF;
}

/*******************************************************************************
 * OVER_CURRENT_EXIT2_IRQHandler
 * 过电流中断处理函数
 *******************************************************************************/
void OVER_CURRENT_EXIT2_IRQHandler(void *arg)
{
    (void)arg;
    msg_t msg;
    if(NVIC_GetActive(EXTI2_IRQn) != 0) {
        NVIC_ClearPendingIRQ(EXTI2_IRQn);
        disable_over_current_irq();
        printf("over current irq...\r\n");
        msg_send(&msg, over_current_pid);
        over_current_event_happen_notify();
    }
}

/*************************************************************
 * 过电流事件信息维护函数
 * ***********************************************************/
void init_channel_over_current_event_info(void)
{
    memset(channel_over_current_event_info, 0x0, sizeof(channel_over_current_event_info));
}

over_current_event_info_t *get_channel_over_current_event_info(uint8_t channel)
{
    return &channel_over_current_event_info[channel];
}

uint32_t get_over_current_event_timestamp(uint8_t channel)
{
    return daq_spi_chan_cnt_since_plus(channel);
}

int get_over_current_max(uint8_t channel)
{
    return daq_spi_get_dat_max(channel);
}

int get_over_current_avr(uint8_t channel)
{
    return daq_spi_get_dat_avr(channel);
}

void update_channel_over_current_event_info(uint8_t channel)
{
    uint16_t sn = 1;
    int max_value = 0;
    int avr_value = 0;

    set_channel_over_current_happened_timestamp(channel, get_over_current_event_timestamp(channel));
    set_channel_over_current_event_sn_info(channel, sn);
    max_value = get_over_current_max(channel);
    set_channel_over_current_max_value(channel, max_value);
    avr_value = get_over_current_avr(channel);
    set_channel_over_current_avr_value(channel, avr_value);
//    set_channel_over_current_k_coefficient(channel, max_value > 2048 ? 1 : -1);
    set_channel_over_current_happened_flag(channel);
}

int check_over_current_sample_done(uint8_t channel)
{
    return daq_spi_sample_done_check(channel);
}

uint32_t get_over_current_sample_length(uint8_t channel)
{
    return daq_spi_get_data_len(channel);
}

void read_over_current_sample_data(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    daq_spi_sample_data_read(channel, data, addr, byte_len);
}

void clear_over_current_sample_done_flag(uint8_t channel)
{
    daq_spi_clear_data_done_flag(channel);
}

#define MAX_FPGA_DATA_LEN (4096) //4k
static uint16_t over_current_event_buffer[MAX_FPGA_DATA_LEN];

void init_over_current_irq(void)
{
    over_current_event_pins_init();
    enable_over_current_irq();
}

void set_default_threshold_rate(void)
{
    uint16_t default_threshold = 2047;
    uint16_t default_changerate = 4095;

    for (int i = 0; i < MAX_OVER_CURRENT_CHANNEL_COUNT; i++) {
        over_current_set_threshold(i, default_threshold);
        over_current_set_changerate(i, default_changerate);
        LOG_INFO("Set over current threshold and changerate for Channel %d: 0x%04X ,0x%04X", i, default_threshold,
                 default_changerate);
    }
}

static void *over_current_event_service(void *arg)
{
    (void)arg;
    msg_t recv_msg;
    uint8_t channel = 0;
    uint32_t length = 0;

    init_over_current_irq();
    init_channel_over_current_event_info();
    set_default_threshold_rate();

    while (1) {
        msg_receive(&recv_msg);
        for (channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            if (0 < check_over_current_sample_done(channel)) {
                if ((length = get_over_current_sample_length(channel)) > MAX_FPGA_DATA_LEN) {
                    LOG_WARN("Get over current curve data length:%ld > MAX_FPGA_DATA_LEN, ignore it.", length);
                    continue;
                }
                set_over_current_curve_data_len(channel, length);
                read_over_current_sample_data(channel, (uint8_t*)over_current_event_buffer, 0, length);
                LOG_INFO("over current of channel:%d curve data length:%d ", channel, length);
                clear_over_current_sample_done_flag(channel);
                update_channel_over_current_event_info(channel);
            }
        }
    }
    /* never reached */
    return NULL;
}

/**
 * @brief   Stack for the over current thread
 */
static char _stack[OVER_CURRENT_STACKSIZE * 3];
kernel_pid_t over_current_service_init(void)
{
	if (over_current_pid == KERNEL_PID_UNDEF) {
		over_current_pid = thread_create(_stack, sizeof(_stack),
				OVER_CURRENT_PRIO, THREAD_CREATE_STACKTEST,
				over_current_event_service, NULL, "overcurrent");
	}
   return over_current_pid;
}

/*************************************************************
 * 过电流事件中断设置信息函数
 * ***********************************************************/
int over_current_event_happen_notify(void)
{
    set_over_current_event_flag();
    return 0;
}

int over_current_get_event_happened_channels(uint8_t *channels, uint8_t *count)
{
   uint8_t channel = 0;
   uint16_t happened_channel_count = 0;
   uint16_t number = 0;

   for (channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
      if (FLAG_ON == get_channel_over_current_happened_flag(channel)) {
         channels[number] = channel;
         happened_channel_count++;
         number++;
      }
   }

   *count = happened_channel_count;

   return 0;
}

int over_current_get_event_info(uint8_t channel, over_current_event_info_t *event)
{
   event->timestamp = channel_over_current_event_info[channel].timestamp;
   event->sn = channel_over_current_event_info[channel].sn;
   event->k = channel_over_current_event_info[channel].k;
   event->max_value = channel_over_current_event_info[channel].max_value;
   return 0;
}

int over_current_set_threshold(uint8_t channel, uint16_t threshold)
{
    daq_spi_set_threshold(channel,threshold);
   return 0;
}

int over_current_set_changerate(uint8_t channel, uint16_t changerate)
{
    daq_spi_set_change_rate(channel,changerate);
   return 0;
}

uint16_t over_current_get_curve_data_len(uint8_t channel,  uint32_t timestamp, uint32_t *length)
{
    uint32_t ret = 0;
    if (get_channel_over_current_happened_timestamp(channel) < timestamp) {
        LOG_WARN("No Over current Curve yet: Request future timestamp:%d ", timestamp);
        return -1;
    }

    *length = get_over_current_curve_data_len(channel);

    return ret;
}
