#include "hf_over_current.h"

#include "fault_location_threads.h"
#include <stdint.h>
#include <string.h>
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

kernel_pid_t hf_over_current_pid = KERNEL_PID_UNDEF;

/*******************over current event pin irq *******************/
#define  HF_OVER_CURRENT_EVENT_PIN  GPIO_PIN(PORT_E,2)

void enable_hf_over_current_irq(void)
{
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void disable_hf_over_current_irq(void)
{
    NVIC_DisableIRQ(EXTI2_IRQn);
}

void OVER_CURRENT_EXIT2_IRQHandler(void *arg)
{
    (void)arg;
    msg_t msg;
    if(NVIC_GetActive(EXTI2_IRQn) != 0) {
        NVIC_ClearPendingIRQ(EXTI2_IRQn);
        msg_send(&msg, hf_over_current_pid);
    }
}

void hf_over_current_event_pins_init(void)
{
    gpio_clear(HF_OVER_CURRENT_EVENT_PIN);
    gpio_init_int(HF_OVER_CURRENT_EVENT_PIN, GPIO_IN, GPIO_RISING, OVER_CURRENT_EXIT2_IRQHandler, NULL);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    disable_hf_over_current_irq();
}

/********************over current event******************************/
hf_over_current_info_t g_hf_over_current_info[MAX_HF_OVER_CURRENT_CHANNEL_COUNT] = { {0} };
hf_over_current_data_t g_hf_over_current_data = {0};

/******************over current info*******************/
void set_hf_over_current_threshold(uint8_t channel, uint16_t threshold)
{
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin_acquire(phase);
        daq_spi_set_hf_threshold(channel, threshold);
        g_hf_over_current_info[channel].threshold = threshold;
        change_spi_cs_pin_release();
    }
//    cfg_set_device_threshold(channel+2, threshold);
}

void set_hf_over_current_changerate(uint8_t channel, uint16_t changerate)
{
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin_acquire(phase);
        daq_spi_set_hf_change_rate(channel, changerate);
        g_hf_over_current_info[channel].change_rate = changerate;
        change_spi_cs_pin_release();
    }
//    cfg_set_device_changerate(channel+2, changerate);
}

hf_over_current_info_t *get_hf_over_current_info(uint8_t channel)
{
    return &g_hf_over_current_info[channel];
}

/********************over current data******************/
int get_hf_over_current_max(uint8_t channel)
{
    return daq_spi_get_dat_max(channel);
}

int get_hf_over_current_avr(uint8_t channel)
{
    return daq_spi_get_dat_avr(channel);
}

uint32_t read_hf_over_current_sample_length(uint8_t channel)
{
    return daq_spi_get_data_len(channel);
}

uint32_t read_hf_over_current_ns_cnt(uint8_t channel)
{
    return daq_spi_chan_cnt_since_plus(channel);
}

void read_hf_over_current_sample_data(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    daq_spi_sample_data_read(channel, data, addr, byte_len);
}

hf_over_current_data_t *get_hf_over_current_data(void)
{
    return &g_hf_over_current_data;
}

/**************************end*********************/

int check_hf_over_current_sample_done(uint8_t channel)
{
    return daq_spi_sample_done_check(channel);
}

void clear_hf_over_current_sample_done_flag(uint8_t channel)
{
    daq_spi_clear_data_done_flag(channel);
}

void trigger_sample_hf_over_current_by_hand(void)
{
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin_acquire(phase);
        for (uint8_t channel = 0; channel < MAX_HF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            daq_spi_trigger_sample(channel);
        }
        change_spi_cs_pin_release();
    }
    set_server_call_flag(HF_DATA);
}

void set_default_threshold_rate(void)
{
    uint16_t default_threshold[2] = { 200, 200 }; //cfg_get_device_high_channel_threshold(channel)
    uint16_t default_changerate[2] = { 4095, 4095 }; //cfg_get_device_high_channel_changerate(channel)cfg_get_device_high_channel_changerate(channel)

    for (int channel = 0; channel < MAX_HF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
        set_hf_over_current_threshold(channel, default_threshold[channel]);
        set_hf_over_current_changerate(channel, default_changerate[channel]);
        LOG_INFO("Set over current threshold and changerate for Channel %d: %d ,%d", channel,
                 default_threshold[channel], default_changerate[channel]);
    }
}

void init_hf_over_current_irq(void)
{
    hf_over_current_event_pins_init();
    enable_hf_over_current_irq();
}

uint16_t get_fpga_uint16_data(uint16_t data)
{
    uint16_t fpga_data = 0;
    uint16_t data_h = (data >> 8)&0xFF;
    uint16_t data_l = data & 0xFF;

    fpga_data = data_l << 8 | data_h;
    return fpga_data;
}

static void *hf_over_current_event_service(void *arg)
{
    (void)arg;
    uint8_t channel = 0;
    uint32_t length = 0;
    uint8_t send_type = 0;

    set_default_threshold_rate();

    while (1) {
        send_type = get_send_type(HF_DATA);
        for (uint8_t phase = 0; phase < 3; phase++) {
            change_spi_cs_pin_acquire(phase);
            for (channel = 0; channel < MAX_HF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
                if (0 < check_hf_over_current_sample_done(channel)) {
                    if ((length = read_hf_over_current_sample_length(channel)) > MAX_FPGA_DATA_LEN) {
                        LOG_WARN("Get over current curve data length:%ld > MAX_FPGA_DATA_LEN, ignore it.", length);
                        continue;
                    }

                    LOG_INFO("channel %d utc reg value %d", channel, daq_spi_chan_event_utc(channel));
                    LOG_INFO("channel %d cnt_since_plus reg value %d", channel, daq_spi_chan_cnt_since_plus(channel));
                    LOG_INFO("channel %d one_sec_clk_cnt reg value %d", channel, daq_spi_one_sec_clk_cnt(channel));
                    g_hf_over_current_data.phase = get_cur_fpga_cs();
                    g_hf_over_current_data.timestamp = daq_spi_chan_event_utc(channel);
                    g_hf_over_current_data.one_sec_clk_cnt = daq_spi_one_sec_clk_cnt(channel);
                    g_hf_over_current_data.curve_len = length;
                    g_hf_over_current_data.ns_cnt = read_hf_over_current_ns_cnt(channel);
                    read_hf_over_current_sample_data(channel, (uint8_t*)g_hf_over_current_data.curve_data, 0, length);
//                    printf("read data is :\r\n");
//                    for (uint16_t i = 0; i < length / 2; i++) {
//                        g_hf_over_current_data.curve_data[i] = get_fpga_uint16_data(
//                                        g_hf_over_current_data.curve_data[i]);
//                        if (i + 1 == 20) printf("\r\n");
//                        printf("%04x ", g_hf_over_current_data.curve_data[i]);
//                    }
                    LOG_INFO("channel %d length is %d ns cnt is %ld", channel, length, g_hf_over_current_data.ns_cnt);
                    if (get_ec20_link_flag()) {
                        send_hf_over_current_curve(&g_hf_over_current_data, channel, send_type);
                    }
                    memset(&g_hf_over_current_data, 0, sizeof(hf_over_current_data_t));
                    clear_hf_over_current_sample_done_flag(channel);
                }
            }
            change_spi_cs_pin_release();
        }
        delay_ms(200);
    }
    return NULL;
}

static char hf_over_current_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t hf_over_current_service_init(void)
{
	if (hf_over_current_pid == KERNEL_PID_UNDEF) {
		hf_over_current_pid = thread_create(hf_over_current_stack, sizeof(hf_over_current_stack),
				HF_OVER_CURRENT_PRIO, THREAD_CREATE_STACKTEST,
				hf_over_current_event_service, NULL, "hf over current");
	}
   return hf_over_current_pid;
}

