/*
 * data_processor.c
 *
 *  Created on: Apr 16, 2019
 *      Author: xuke
 */
#include <stdint.h>
#include <math.h>

#include "thread.h"
#include "periph/rtt.h"
#include "vc_temp_bat_vol_sample.h"
#include "periph/stm32f10x_std_periph.h"
#include "timex.h"
#include "xtimer.h"
#include "log.h"
#include "dev_cfg.h"
#include "adc_gain_set.h"

__IO uint16_t sample_buf[CHANNEL_COUNT * SAMPLE_COUNT];

uint16_t vc_adc1_channel_list[CHANNEL_COUNT] = { 8, 9, 10, 0, 4, 1, 11, 12, 13 };
uint16_t temp_adc2_channel_list[TEMP_ADC2_CHAN_CNT] = { 14, 15 }; //temp A , temp B
uint16_t temp_bat_adc3_channel_list[TEMP_BAT_ADC3_CHAN_CNT] = { 4, 5 }; //tempC, bat

uint16_t temp_channel[3] = {14, 15, 4};
uint16_t bat_val_channel = 5;

static phase_t phase = PHASE_A;

static uint32_t vc_do_sample_time_start = 0;
static uint32_t vc_do_sample_time_now = 0;

static uint32_t temp_do_sample_time_start = 0;
static uint32_t temp_do_sample_time_now = 0;

static uint32_t bat_do_sample_time_start = 0;
static uint32_t bat_do_sample_time_now = 0;

static kernel_pid_t to_receive_pid;

static float rms_data[CHANNEL_COUNT] = { 0 };
static uint16_t temp[3] = { 0 };
static uint16_t bat;

msg_t sample_done_msg;

void delay_s(int s)
{
    xtimer_ticks32_t last_wakeup = xtimer_now();
    xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
}

void delay_ms(int ms)
{
    xtimer_ticks32_t last_wakeup = xtimer_now();
    xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}

void calc_rms(uint16_t *raw_data, float *rms_data)
{
    uint32_t quadratic_sum[CHANNEL_COUNT] = { 0 };

    for (int channel = 0; channel < CHANNEL_COUNT; channel++) {
        for (int i = 0; i < SAMPLE_COUNT; i++) {
            quadratic_sum[channel] += (uint32_t)((raw_data[CHANNEL_COUNT * i + channel] - 2048)
                            * (raw_data[CHANNEL_COUNT * i + channel] - 2048));
        }
        rms_data[channel] = sqrt((double)quadratic_sum[channel] / SAMPLE_COUNT);
        if (channel < 3) {
            rms_data[channel] = rms_data[channel] / 4096 * 3.3 / 10 / 10 * 1000;
        }
        else if (channel < 6) {
            rms_data[channel] = rms_data[channel] / 4096 * 3.3 / 500 * 250000;
        }
        else if (channel < 9) {
            rms_data[channel] = rms_data[channel] / 4096 * 3.3 / 100 / 105 * 1000 * 1000 / 10;
        }
    }
}

static void vc_sample_init(void)
{
    adc1_dma1_tim4_sample_init(vc_adc1_channel_list, CHANNEL_COUNT, SAMPLE_COUNT, SAMPLE_DURATION_MS, (uint16_t*)sample_buf);
}

static void vc_sample_enable(void)
{
    adc1_dma1_tim4_sample_enable();
}

static void temp_bat_sample_init(void)
{
    adc2_sample_init(temp_adc2_channel_list, TEMP_ADC2_CHAN_CNT);
    adc3_sample_init(temp_bat_adc3_channel_list, TEMP_BAT_ADC3_CHAN_CNT);
}

static void interval_do_vc_sample(void)
{
    vc_do_sample_time_now = rtt_get_counter();

    mx_sensor_info_t* cur_info = get_dev_cur_info_by_phase(phase);
    if(vc_do_sample_time_start == 0){
        vc_do_sample_time_start = vc_do_sample_time_now;
    }
    if (vc_do_sample_time_now - vc_do_sample_time_start > cur_info->interval*60) {
    	vc_do_sample_time_start = vc_do_sample_time_now;
        vc_sample_init();
        vc_sample_enable();
        delay_ms(500);
        calc_rms((uint16_t*)sample_buf, rms_data);
        sample_done_msg.content.ptr = (void*)rms_data;
        msg_send(&sample_done_msg, to_receive_pid);
    }
}

static void interval_do_temp_sample(void)
{
    temp_do_sample_time_now = rtt_get_counter();
    mx_sensor_info_t* temp_info = get_dev_temp_info_by_phase(phase);

    if (temp_do_sample_time_now - temp_do_sample_time_start > temp_info->interval*60) {
        temp_bat_sample_init();
        for (int i = 0; i < 3; i++) {
            temp[i] = adc2_sample_by_channel(temp_channel[i]);
            if (i == 2) {
                temp[i] = adc3_sample_by_channel(temp_channel[i]);
            }
        }
        temp_do_sample_time_start = temp_do_sample_time_now;
    }
}

static void interval_do_bat_sample(void)
{
    bat_do_sample_time_now = rtt_get_counter();
    if (bat_do_sample_time_now - bat_do_sample_time_start > 10*60) {
        temp_bat_sample_init();
        bat = adc3_sample_by_channel(bat_val_channel);
        bat_do_sample_time_start = bat_do_sample_time_now;
    }
}

void  do_receive_pid_hook(kernel_pid_t pid)
{
	to_receive_pid = pid;
}

uint16_t get_line_temp(line_temperature_t line)
{
    return temp[line];
}

uint16_t get_bat_vol(void)
{
    return bat;
}

void *vc_temp_bat_sample_serv(void *arg)
{
    (void)arg;
    set_default_gain_for_all_chan();
    while (1) {
        interval_do_vc_sample();
        interval_do_temp_sample();
        interval_do_bat_sample();
        delay_ms(500);
    }
}

#define VC_TEMP_BAT_SAMPLE_SERV_PRIORITY  10
static char vc_temp_bat_sample_thread_stack[THREAD_STACKSIZE_MAIN * 2];


kernel_pid_t vc_temp_bat_sample_serv_init(void)
{
    kernel_pid_t _pid = thread_create(vc_temp_bat_sample_thread_stack, sizeof(vc_temp_bat_sample_thread_stack),
                                      VC_TEMP_BAT_SAMPLE_SERV_PRIORITY, THREAD_CREATE_STACKTEST, vc_temp_bat_sample_serv, NULL,
                                      "vc temp bat sample serv");
    return _pid;

}

