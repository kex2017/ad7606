#include "over_current.h"

#include <stdint.h>
#include <string.h>
#include "env_cfg.h"
#include "log.h"
#include "board.h"
#include "periph/rtt.h"
#include "thread.h"
#include "x_delay.h"
#include "periph/gpio.h"
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
        disable_over_current_irq();
        printf("over current irq...\r\n");
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
over_current_data_t g_over_current_data[MAX_OVER_CURRENT_CHANNEL_COUNT] = { {0} };

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

void set_over_current_ns_cnt(uint8_t channel)
{
    g_over_current_data[channel].ns_cnt = daq_spi_chan_cnt_since_plus(channel);
}

void read_over_current_sample_data(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    daq_spi_sample_data_read(channel, data, addr, byte_len);
}

over_current_data_t *get_over_current_data(uint8_t channel)
{
    return &g_over_current_data[channel];
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

void set_default_threshold_rate(void)
{
    uint16_t default_threshold = 2047;
    uint16_t default_changerate = 4095;

    for (int channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
        set_over_current_threshold(channel, default_threshold);
        set_over_current_changerate(channel, default_changerate);
        LOG_INFO("Set over current threshold and changerate for Channel %d: 0x%04X ,0x%04X", channel, default_threshold,
                 default_changerate);
    }
}

void init_over_current_irq(void)
{
    over_current_event_pins_init();
    enable_over_current_irq();
}

static void *over_current_event_service(void *arg)
{
    (void)arg;
    msg_t recv_msg;
    uint8_t channel = 0;
    uint32_t length = 0;

    init_over_current_irq();
    set_default_threshold_rate();

    while (1) {
        msg_receive(&recv_msg);
        for (channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            if (0 < check_over_current_sample_done(channel)) {
                if ((length = read_over_current_sample_length(channel)) > MAX_FPGA_DATA_LEN) {
                    LOG_WARN("Get over current curve data length:%ld > MAX_FPGA_DATA_LEN, ignore it.", length);
                    continue;
                }
                set_over_current_ns_cnt(channel);
                g_over_current_data[channel].curve_len = length;
                read_over_current_sample_data(channel, (uint8_t*)(g_over_current_data[channel].curve_data), 0, length);
                LOG_INFO("over current of channel:%d curve data length:%d ", channel, length);
                clear_over_current_sample_done_flag(channel);
            }
        }
        enable_over_current_irq();
    }
    /* never reached */
    return NULL;
}

static char over_current_stack[THREAD_STACKSIZE_MAIN * 2];
kernel_pid_t over_current_service_init(void)
{
	if (over_current_pid == KERNEL_PID_UNDEF) {
		over_current_pid = thread_create(over_current_stack, sizeof(over_current_stack),
				OVER_CURRENT_PRIO, THREAD_CREATE_STACKTEST,
				over_current_event_service, NULL, "over current");
	}
   return over_current_pid;
}

