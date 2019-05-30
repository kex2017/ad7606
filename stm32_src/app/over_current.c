#include "over_current.h"
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
#include "daq.h"

kernel_pid_t over_current_pid = KERNEL_PID_UNDEF;

/*******************over current event pin irq *******************/
#define  OVER_CURRENT_EVENT_PIN  GPIO_PIN(PORT_E,2)

void enable_over_current_irq(void)
{
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void disable_over_current_irq(void)
{
    NVIC_DisableIRQ(EXTI2_IRQn);
}

void OVER_CURRENT_EXIT2_IRQHandler(void *arg)
{
    (void)arg;
    msg_t msg;
    if(NVIC_GetActive(EXTI2_IRQn) != 0) {
        NVIC_ClearPendingIRQ(EXTI2_IRQn);
        msg_send(&msg, over_current_pid);
    }
}

void over_current_event_pins_init(void)
{
    gpio_clear(OVER_CURRENT_EVENT_PIN);
    gpio_init_int(OVER_CURRENT_EVENT_PIN, GPIO_IN, GPIO_RISING, OVER_CURRENT_EXIT2_IRQHandler, NULL);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    disable_over_current_irq();
}

/********************over current event******************************/
over_current_info_t g_over_current_info[MAX_OVER_CURRENT_CHANNEL_COUNT] = { {0} };
over_current_data_t g_over_current_data = {0};

/******************over current info*******************/
void set_over_current_threshold(uint8_t channel, uint16_t threshold)
{
    daq_spi_set_threshold(channel, threshold);
    g_over_current_info[channel].threshold = threshold;
    cfg_set_device_threshold(channel+2, threshold);
}

void set_over_current_changerate(uint8_t channel, uint16_t changerate)
{
    daq_spi_set_change_rate(channel, changerate);
    g_over_current_info[channel].change_rate = changerate;
    cfg_set_device_changerate(channel+2, changerate);
}

void set_over_current_cal_k_b(uint8_t channel, cal_k_b_t cal_k_b)
{
    g_over_current_info[channel].cal_k_b = cal_k_b;
}

cal_k_b_t get_over_current_cal_k_b(uint8_t channel)
{
    return g_over_current_info[channel].cal_k_b;
}

over_current_info_t *get_over_current_info(uint8_t channel)
{
    return &g_over_current_info[channel];
}

/********************over current data******************/
int get_over_current_max(uint8_t channel)
{
    return daq_spi_get_dat_max(channel);
}

int get_over_current_avr(uint8_t channel)
{
    return daq_spi_get_dat_avr(channel);
}

uint32_t read_over_current_sample_length(uint8_t channel)
{
    return daq_spi_get_data_len(channel);
}

uint32_t read_over_current_ns_cnt(uint8_t channel)
{
    return daq_spi_chan_cnt_since_plus(channel);
}

void read_over_current_sample_data(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    daq_spi_sample_data_read(channel, data, addr, byte_len);
}

over_current_data_t *get_over_current_data(void)
{
    return &g_over_current_data;
}

/**************************end*********************/

int check_over_current_sample_done(uint8_t channel)
{
    return daq_spi_sample_done_check(channel);
}

void clear_over_current_sample_done_flag(uint8_t channel)
{
    daq_spi_clear_data_done_flag(channel);
}

void trigger_sample_over_current_by_hand(uint8_t channel)
{
    daq_spi_trigger_sample(channel);
    set_server_call_flag(1);
}

void set_default_threshold_rate(void)
{
    uint16_t default_threshold[2] = { 200, 200 }; //cfg_get_device_high_channel_threshold(channel)
    uint16_t default_changerate[2] = { 4095, 4095 }; //cfg_get_device_high_channel_changerate(channel)cfg_get_device_high_channel_changerate(channel)

    for (int channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
        set_over_current_threshold(channel, default_threshold[channel]);
        set_over_current_changerate(channel, default_changerate[channel]);
        LOG_INFO("Set over current threshold and changerate for Channel %d: %d ,%d", channel,
                 default_threshold[channel], default_changerate[channel]);
    }
}

void init_over_current_irq(void)
{
    over_current_event_pins_init();
    enable_over_current_irq();
}

uint16_t get_fpga_uint16_data(uint16_t data)
{
    uint16_t fpga_data = 0;
    uint16_t data_h = (data >> 8)&0xFF;
    uint16_t data_l = data & 0xFF;

    fpga_data = data_l << 8 | data_h;
    return fpga_data;
}

static void *over_current_event_service(void *arg)
{
    (void)arg;
//    msg_t recv_msg;
    uint8_t channel = 0;
    uint32_t length = 0;
    uint8_t send_type = 0;

    init_over_current_irq();
    set_default_threshold_rate();

    while (1) {
        send_type = get_send_type();
        for (channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            if (0 < check_over_current_sample_done(channel)) {
                if ((length = read_over_current_sample_length(channel)) > MAX_FPGA_DATA_LEN) {
                    LOG_WARN("Get over current curve data length:%ld > MAX_FPGA_DATA_LEN, ignore it.", length);
                    continue;
                }
                g_over_current_data.curve_len = length;
                read_over_current_sample_data(channel, (uint8_t*)g_over_current_data.curve_data, 0, length);
//                printf("read data is :\r\n");
                for(uint16_t i = 0; i < length / 2; i++){
                    g_over_current_data.curve_data[i] = get_fpga_uint16_data(g_over_current_data.curve_data[i]);
//                    if(i+1 == 20)
//                    printf("\r\n");
//                    printf("%04x ",g_over_current_data.curve_data[i]);
                }
                g_over_current_data.ns_cnt = read_over_current_ns_cnt(channel);
                LOG_INFO("channel %d length is %d ns cnt is %ld\r\n", channel, length, g_over_current_data.ns_cnt);

                send_over_current_curve(&g_over_current_data, channel+2, send_type);
                memset(&g_over_current_data, 0, sizeof(over_current_data_t));
                clear_over_current_sample_done_flag(channel);
            }
        }
        delay_ms(200);
    }
    return NULL;
}

static char over_current_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t over_current_service_init(void)
{
	if (over_current_pid == KERNEL_PID_UNDEF) {
		over_current_pid = thread_create(over_current_stack, sizeof(over_current_stack),
				OVER_CURRENT_PRIO, THREAD_CREATE_STACKTEST,
				over_current_event_service, NULL, "over current");
	}
   return over_current_pid;
}

