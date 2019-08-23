#include "pf_over_current.h"

#include "fault_location_threads.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "env_cfg.h"
#include "log.h"
#include "board.h"
#include "periph/rtt.h"
#include "thread.h"
#include "x_delay.h"
#include "periph/gpio.h"
#include "data_send.h"
#include "data_transfer.h"
#include "daq.h"

kernel_pid_t pf_over_current_pid = KERNEL_PID_UNDEF;

/********************over current event******************************/
pf_over_current_info_t g_pf_over_current_info[MAX_PF_OVER_CURRENT_CHANNEL_COUNT] = { {0} };
pf_over_current_data_t g_pf_over_current_data = {0};

/******************over current info*******************/
void set_pf_over_current_threshold(uint8_t channel, uint16_t threshold)
{
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin(phase);
        daq_spi_set_pf_threshold(channel, threshold * threshold * 512);
        g_pf_over_current_info[channel].threshold = threshold;
    }
//    cfg_set_device_threshold(channel+2, threshold);
}

pf_over_current_info_t *get_pf_over_current_info(uint8_t channel)
{
    return &g_pf_over_current_info[channel];
}

/********************over current data******************/
float get_pf_over_current_rms(uint8_t channel)
{
    long pf_sum = daq_spi_get_pf_sum_data(channel);
    printf("pf_sum channel %d is %ld \r\n", channel, pf_sum);
    uint16_t sample_cnt = daq_spi_get_data_len(channel)/2;
    printf("sample cnt channel %d is %d \r\n", channel, sample_cnt);

    return sqrt((double)pf_sum / sample_cnt);
}

uint32_t read_pf_over_current_sample_length(uint8_t channel)
{
    return daq_spi_get_data_len(channel);
}

uint32_t read_pf_over_current_ns_cnt(uint8_t channel)
{
    return daq_spi_chan_cnt_since_plus(channel);
}

void read_pf_over_current_sample_data(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    daq_spi_sample_data_read(channel, data, addr, byte_len);
}

pf_over_current_data_t *get_pf_over_current_data(void)
{
    return &g_pf_over_current_data;
}

/**************************end*********************/

int check_pf_over_current_sample_done(uint8_t channel)
{
    return daq_spi_sample_done_check(channel);
}

void clear_pf_over_current_sample_done_flag(uint8_t channel)
{
    daq_spi_clear_data_done_flag(channel);
}

void trigger_sample_pf_over_current_by_hand(void)
{
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin(phase);
        for (uint8_t channel = PF_CHANNEL_OFFSET; channel < PF_CHANNEL_OFFSET + MAX_PF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            daq_spi_trigger_sample(channel);
        }
    }
    set_server_call_flag(PF_DATA);
}

void set_pf_default_threshold(void)
{
    uint16_t default_threshold = 200;

    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin(phase);
        for (int channel = PF_CHANNEL_OFFSET; channel < PF_CHANNEL_OFFSET + MAX_PF_OVER_CURRENT_CHANNEL_COUNT;
                        channel++) {
            set_pf_over_current_threshold(channel, default_threshold);
            LOG_INFO("Set pf over current threshold for Channel %d: %d", channel, default_threshold);
        }
    }
}

static void *pf_over_current_event_service(void *arg)
{
    (void)arg;
    uint8_t channel = 0;
    uint32_t length = 0;
    uint8_t send_type = 0;

    set_pf_default_threshold();

    while (1) {
        send_type = get_send_type(PF_DATA);
        for (uint8_t phase = 0; phase < 3; phase++) {
            change_spi_cs_pin(phase);
            for (channel = PF_CHANNEL_OFFSET; channel < PF_CHANNEL_OFFSET + MAX_PF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
                if (0 < check_pf_over_current_sample_done(channel)) {
                    if ((length = read_pf_over_current_sample_length(channel)) > MAX_PF_FPGA_DATA_LEN) {
                        LOG_WARN("Get pf over current curve data length:%ld > MAX_PF_FPGA_DATA_LEN, ignore it.",
                                 length);
                        continue;
                    }

                    LOG_INFO("channel %d utc reg value %d", channel, daq_spi_chan_event_utc(channel));
                    LOG_INFO("channel %d cnt_since_plus reg value %d", channel, daq_spi_chan_cnt_since_plus(channel));
                    LOG_INFO("channel %d one_sec_clk_cnt reg value %d", channel, daq_spi_one_sec_clk_cnt(channel));
                    g_pf_over_current_data.phase = get_cur_fpga_cs();
                    g_pf_over_current_data.timestamp = daq_spi_chan_event_utc(channel);
                    g_pf_over_current_data.one_sec_clk_cnt = daq_spi_one_sec_clk_cnt(channel);
                    g_pf_over_current_data.curve_len = length;
                    g_pf_over_current_data.ns_cnt = read_pf_over_current_ns_cnt(channel);
                    read_pf_over_current_sample_data(channel, (uint8_t*)g_pf_over_current_data.curve_data, 0, length);
//                printf("read data is :\r\n");
//                for(uint16_t i = 0; i < length / 2; i++){
                    // g_pf_over_current_data.curve_data[i] = get_fpga_uint16_data(g_pf_over_current_data.curve_data[i]);
//                    if(i+1 == 20)
//                    printf("\r\n");
//                    printf("%04x ",g_pf_over_current_data.curve_data[i]);
//                }
                    LOG_INFO("channel %d length is %d ns cnt is %ld", channel, length, g_pf_over_current_data.ns_cnt);
                    if (get_ec20_link_flag()) {
                        send_pf_over_current_curve(&g_pf_over_current_data, channel, send_type);
                    }
                    memset(&g_pf_over_current_data, 0, sizeof(pf_over_current_data_t));
                    clear_pf_over_current_sample_done_flag(channel);
                }
            }
        }
        delay_ms(200);
    }
    return NULL;
}

static char pf_over_current_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t pf_over_current_service_init(void)
{
	if (pf_over_current_pid == KERNEL_PID_UNDEF) {
		pf_over_current_pid = thread_create(pf_over_current_stack, sizeof(pf_over_current_stack),
		                                    PF_OVER_CURRENT_PRIO, THREAD_CREATE_STACKTEST,
		                                    pf_over_current_event_service, NULL, "pf over current");
	}
   return pf_over_current_pid;
}

