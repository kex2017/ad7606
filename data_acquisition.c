/*
 * data_acquisition.c
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#include <string.h>
#include <math.h>
#include <time.h>

#include "log.h"
#include "x_delay.h"
#include "type_alias.h"

#include "msg.h"
#include "board.h"
#include "periph/rtt.h"
#include "daq.h"
#include "data_acquisition.h"
#include "acquisition_trigger.h"
#include "upgrade_from_sd_card.h"
#include "curve_harmonics.h"
#include "data_storage.h"

#include "partial_discharge.h"
#include "over_voltage.h"

#include "cable_ground_threads.h"


/****************************************************
 * cable ground logic channel statement
 * FSMC FPGA
 * 0-3: 高频电流
 * 4-9: 介损/工频电流
 * 10-12: 局放原始波形
 * 13-15: PRPD
 * SPI FPGA
 * 0-3: 过压事件
 *****************************************************/
static uint8_t g_sample_data[MAX_CURVE_DATA_LEN] __attribute__((section(".big_data")));
uint32_t data_acquisition_set_trigger_sammple_info(uint32_t timestamp, uint8_t channel_count, void* cfg)
{
    uint8_t index = 0;
    channel_cfg_t cfg_info;
    channel_cfg_t *tmp = (channel_cfg_t*)cfg;
    uint32_t seconds = rtt_get_counter();

    if (seconds > 3600) {//not 1970-01-01
        set_manual_sample_timer_trigger(timestamp - seconds, MSG_READ_COMMON_AND_PRPD);
        LOG_INFO("Total sample channel count:%d for timestamp:%ld.", channel_count, timestamp);
    }
    else {
        set_manual_sample_timer_trigger(10, MSG_READ_COMMON_AND_PRPD);
        LOG_WARN("Device time is invalid, try to trigger sample after 10 seconds.");
    }

    for (index = 0; index < channel_count; index++) {
        cfg_info = *tmp++; //test this usage there
        set_channel_sample_cfg(cfg_info);
        //set parameters to FPGA device
        if (0 == cfg_info.channel || 1 == cfg_info.channel || 2 == cfg_info.channel || 3 == cfg_info.channel) {
            hal_JF_gain_out(cfg_info.channel, cfg_info.gain);
        }

    }
    display_sample_info();

    return 0;
}

int data_acquisition_read_curve_data(uint8_t channel, uint32_t timestamp, uint16_t size, uint16_t cur_pkt_num, void* data, uint16_t *total_pkt_count)
{
    uint32_t ret = 0;
    uint32_t addr;
    uint32_t len = 0;
    uint32_t sample_timestamp = get_channel_sample_timestamp(channel);

    if (timestamp > sample_timestamp) {//TODO: check if this condition usefull
        LOG_WARN("Master request curve data timestamp expected(%d) > actual(%d)!", timestamp, sample_timestamp);
        return DEVTIMEERR;
    }
    len = get_curve_data_len(channel);
    *total_pkt_count = len / size + !!(len % size);
    if ((*total_pkt_count - 1) == cur_pkt_num || 0 == cur_pkt_num) {
        LOG_INFO("Read saved curve data: channel:%d, length:%d, cur_pkt_num:%d, total_pkt_count:%d OK.", channel, len, cur_pkt_num,  *total_pkt_count);
    }
    addr = get_curve_data_start_addr(channel);
    addr = addr + cur_pkt_num * size;
    ret = get_curve_data(data, addr, size);

    return ret;
}

uint32_t data_acquisition_get_curve_data_len(uint8_t channel, uint32_t timestamp, uint32_t *len)
{
    uint32_t sample_timestamp = get_channel_sample_timestamp(channel);
    if (timestamp > sample_timestamp) { //TODO: check if this condition usefull
        LOG_WARN("Master request curve data timestamp expected(%d) > actual(%d)!", timestamp, sample_timestamp);
        return DEVTIMEERR;
    }
    *len = get_curve_data_len(channel);
    LOG_INFO("Read saved curve data length: channel:%d, length:%u.", channel, *len);

    return 0;
}

/*******************************************************************************
 * get_pf_current_data_from_fpga
 * 获取功频电流波形数据并且计算出abc三相和合相的功频电流值
 *******************************************************************************/
void calc_abc(uint8_t channel, float *dest, uint8_t *src, uint32_t len)
{
    uint32_t i = 0;
    signed short *p = (signed short *)src;
    uint8_t base = 6;

    if (channel > 6) {
        base = 96;
    }

    for (i = 0; i < len / 2; i++) {
        *dest = ((float)(*p) * 2048) / (32768 * base * 10);
        dest++;
        p++;
    }
}

void calc_pf_current_abc(float *value, float *data, uint32_t len)
{
    uint32_t i = 0;
    float tmp = 0;
    float sum = 0;

    for (i = 0; i < len / 2; i++) {
        tmp = *data++;
        sum += tmp * tmp;
    }
    *value = (float)sqrt((float)(sum / (len / 2)));
}

void calc_pf_current_total(float *value, float data[][MAX_FPGA_DATA_LEN/2], uint32_t len)
{
    uint32_t i = 0;

    for (i = 0; i < len / 2; i++) {
        data[3][i] = data[0][i] + data[1][i] + data[2][i];
    }

    calc_pf_current_abc(value, data[3], len);
}

static float pf_current_abc_data[7][MAX_CURVE_DATA_LEN / 2] __attribute__((section(".big_data")));
const int PF_READ_RETRY_TIME = 60;
uint32_t data_acquisition_read_pf_current(uint8_t channel, uint32_t timestamp, void* data)
{
    static uint8_t pf_current_channels[] = { 4, 5, 6, 7, 8, 9 };
    float power_current[7] = { 0.0 };
    uint16_t count = 0;
    uint32_t len = 0;

    (void) timestamp;

    daq_fsmc_multi_chans_sample(pf_current_channels, sizeof(pf_current_channels));
    delay_s(1);
    LOG_INFO("Write trigger register(8) to read PF current.");

    for (channel = 4; channel < 10; channel++) {
        memset(g_sample_data, 0, sizeof(g_sample_data));
        for (count = 0; count < PF_READ_RETRY_TIME; count++) {     //等待fpga采样完成，尝试60次10us的等待,如果采样不完成则返回错误码
            if (0 < daq_fsmc_sample_done_check(channel)) {
                break;
            }
            delay_ms(5);
        }
        if (PF_READ_RETRY_TIME == count) {
            return SAMPLEERR;
        }
        len = daq_fsmc_sample_len_get(channel);
        if (len > MAX_CURVE_DATA_LEN) {
            LOG_ERROR("Get invalid length(%lu) > 24k, drop this pf current curve data.", len);
            return SAMPLEERR;
        }
        daq_fsmc_sample_data_read(channel, g_sample_data, 0, len);
        calc_abc(channel, pf_current_abc_data[channel - 4], g_sample_data, len);
        calc_pf_current_abc(&power_current[channel - 4], pf_current_abc_data[channel - 4], len);
    }
    /* 4，5，6:A1,B1,C1; 7,8,9:A2,B2,C2
     * A1,B1,C1分别于1.5A比较, 当X1>=1.5A时, 取X1电流值, 否则取X2,
     * 并根据最后的三相电流计算合相电流值 */
    LOG_INFO("Read power frequency current: A1:%f, B1:%f, C1:%f.", power_current[0], power_current[1], power_current[2]);
    LOG_INFO("Read power frequency current: A2:%f, B2:%f, C2:%f.", power_current[3], power_current[4], power_current[5]);
    /*channel4~9 data length must be equal @xiewei*/
    if (power_current[0] < 1.5) {
    	power_current[0] = power_current[3];
    	memcpy(&pf_current_abc_data[0], &pf_current_abc_data[3], len / 2);
    }

    if (power_current[1] < 1.5) {
    	power_current[1] = power_current[4];
    	memcpy(&pf_current_abc_data[1], &pf_current_abc_data[4], len / 2);
    }

    if (power_current[2] < 1.5) {
    	power_current[2] = power_current[5];
    	memcpy(&pf_current_abc_data[2], &pf_current_abc_data[5], len / 2);
    }


	calc_pf_current_total(power_current + 3, pf_current_abc_data, len); /*有三个通道的数据长度，我暂时以第6通道的数据长度作为计算基础*/
	LOG_INFO("Read all phase power frequency current: %f.", power_current[3]);
	memcpy(data, (char*)power_current, sizeof(float) * 4);

    return DEVICEOK;
}


uint16_t get_curve_max_value(uint16_t* curve_data, uint32_t size)
{
	uint32_t i = 0;
	uint16_t max = 0;

	for (i = 0; i < size; i++) {
		max = *curve_data > max ? *curve_data : max;
		curve_data++;
	}

	return max;
}

#define REFERENCE_VOLTAGE (1.0)
float calc_jf_current(uint16_t value, uint8_t channel)
{
	uint16_t gain = get_channel_gain_cfg(channel);
	return ((REFERENCE_VOLTAGE * (value - 2048) / 4096)) / (2 * sqrt(2) * gain);
}

uint32_t data_acquisition_read_jf_current(uint8_t channel)
{
    static uint8_t jf_current_channels[] = { 0, 1, 2 };
    uint8_t i = 0, count = 0;
    uint32_t len = 0;
    uint16_t max_value = 0;
    float current;

    daq_fsmc_multi_chans_sample(jf_current_channels, sizeof(jf_current_channels));
    delay_s(1);
    LOG_INFO("Write trigger register(8) to read JF current.");

    for (i = 0; i < 3; i++) {
        channel = jf_current_channels[i];
        memset(g_sample_data, 0, sizeof(g_sample_data));
        for (count = 0; count < PF_READ_RETRY_TIME; count++) {     //等待fpga采样完成，尝试60次10us的等待,如果采样不完成则返回错误码
            if (0 < daq_fsmc_sample_done_check(channel)) {
                break;
            }
            delay_ms(5);
        }
        if (PF_READ_RETRY_TIME == count) {
            return SAMPLEERR;
        }
        len = daq_fsmc_sample_len_get(channel);
        if (len > MAX_CURVE_DATA_LEN) {
            LOG_ERROR("Get invalid length(%lu) > MAX_CURVE_DATA_LEN, drop this pf current curve data.", len);
            return SAMPLEERR;
        }
        daq_fsmc_sample_data_read(channel, g_sample_data, 0, len);
        max_value = get_curve_max_value((uint16_t*)g_sample_data, len / 2);
        current = calc_jf_current(max_value, channel);
        LOG_INFO("Get JF channel(%d) current as %f.", channel, current);
    }

    return 0;
}

#define CALC_HF_CURRENT(max, gain) (((max - 2048)/4096.0 * 10.0) / pow(10, gain/20.0) / 20)
void read_and_save_sample_curve_data(uint8_t channel)
{
    uint32_t length = 0;
    int avg_value = 0;
    uint8_t gain = 0;

    if ((length = daq_fsmc_sample_len_get(channel)) > MAX_CURVE_DATA_LEN) {
    	LOG_ERROR("Get channel:%d data invalid length(%lu) > 24k, drop this common curve data.", channel, length);
        return;
    }
    memset(g_sample_data, 0x0, sizeof(g_sample_data));
    avg_value = daq_fsmc_sample_data_read(channel, g_sample_data, 0, length);
    reset_channel_sample(channel);
    if (channel < 4 && avg_value > 0) {
    	gain = get_channel_gain_cfg(channel);
    	LOG_INFO("Get HF channel:%d curve avg value:%d, gain:%d(DB), HF current:%f.", channel, avg_value, gain, CALC_HF_CURRENT(avg_value, gain));
    }
    else if (channel < 10) {
       LOG_INFO("Try to calc harmonics for channel:%d.", channel);
       calc_dieloss_harmonics((int16_t*)g_sample_data, channel);
    }
    set_curve_data(channel, g_sample_data, length);
    LOG_INFO("Read sample curve data from FPGA, channel:%d, length:%d, 99th:%d, 100th:%d", channel, length, g_sample_data[98], g_sample_data[99]);
    if (get_save_curve_data_to_sd_flag()) {
        save_curve_data_to_sd(channel, g_sample_data, length);
    }
}

void common_curve_data_read(msg_read_info_t* info)
{
    uint8_t index = 0;

    for (index = 0; index < 10; index++) {
        if (info->channels & (0x01 << index)) {
            acquire_fsmc_ops_mtx();
            if (0 < daq_fsmc_sample_done_check(index)) {
                read_and_save_sample_curve_data(index);
                set_channel_sample_timestamp(index, info->timestamp);
            }
            else {
                LOG_WARN("Curve data for channel:%d is not ready yet.", index);
            }
            release_fsmc_ops_mtx();
            delay_ms(10);
        }
    }
}

const uint8_t BASE_PD_PRPD_CHAN =  10;
void read_and_save_prpd_curve_data(uint8_t channel)
{
    uint32_t length = 0;

    if ((length = daq_fsmc_sample_len_get(channel)) > 0) {
        memset(g_sample_data, 0x0, MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN);
        length = (length > MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN) ? MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN : length;
        daq_fsmc_sample_data_read(channel, g_sample_data, 0, length);
        reset_channel_sample(channel);
        set_pd_or_prpd_curve_data(channel - BASE_PD_PRPD_CHAN, 0, (void*)g_sample_data, length);
        if (get_save_curve_data_to_sd_flag()) {
            save_curve_data_to_sd(channel, g_sample_data, length);
        }
    }
    else {
        LOG_WARN("DAQ get PRPD curve data for channel:%d length is 0.", channel);
    }
}

const uint8_t BASE_PRPD_CHAN =  13;
void prpd_curve_data_read_process(msg_read_info_t* info)
{
    uint8_t index = 0;

    for (index = BASE_PRPD_CHAN; index < MAX_CHANNEL; index++) {
        if (info->channels & (0x01 << index)) {
            acquire_fsmc_ops_mtx();
            if (0 < daq_fsmc_sample_done_check(index)) {
                daq_fsmc_prpd_sample_status_set(0);
                read_and_save_prpd_curve_data(index);
                daq_fsmc_prpd_sample_status_set(1);
                set_channel_sample_timestamp(index, info->timestamp);
            }
            else {
                LOG_WARN("PRPD curve data for channel:%d is not ready yet.", index);
            }
            release_fsmc_ops_mtx();
            delay_ms(10);
        }
    }
}

void common_and_prpd_curve_data_read_process(msg_read_info_t *info)
{
    //waiting sample done
    delay_ms(1100);

    common_curve_data_read(info);
    prpd_curve_data_read_process(info);
}

void common_curve_data_read_process(msg_read_info_t *info)
{
    //waiting sample done
    delay_ms(1100);

    common_curve_data_read(info);
}

void pd_event_process(msg_read_info_t *info)
{
    partial_discharge_process_event(info->channels, info->timestamp);
    enable_partial_discharge_irq();
}

void ov_event_process(msg_read_info_t *info)
{
    over_voltage_process_one_event(info->channels, info->timestamp);
    enable_over_voltage_irq();
}

void init_jf_gain(void)
{
	int i = 0;

	for (i = 0; i < 4; i++) {
		hal_JF_gain_out(i, get_channel_gain_cfg(i));
	}
}

void fpga_channel_cfg_init(void)
{
	init_sample_cfg();
	init_jf_gain();
}

#define MAX_RECEIVE_QUEUE (32)
static msg_t trigger_queue[MAX_RECEIVE_QUEUE];

void data_acquisiton_init(void)
{
    daq_init();
    hal_init_da7568();
    fpga_channel_cfg_init();
    pd_and_ov_event_irq_init();
    ov_event_info_channels_init();
    pd_event_info_channels_init();
}

void *data_acquisition_service(void *argv)
{
    (void)argv;
    msg_t msg = { 0 };
    msg_read_info_t info = {0};

    data_acquisiton_init();
    msg_init_queue(trigger_queue, MAX_RECEIVE_QUEUE);

    while (1) {
        msg_receive(&msg);
        info = *(msg_read_info_t*)msg.content.ptr;
        switch (msg.type) {
        case MSG_READ_COMMON_AND_PRPD:
            common_and_prpd_curve_data_read_process(&info);
            break;
        case MSG_READ_COMMON:
            common_curve_data_read_process(&info);
            break;
        case MSG_READ_PRPD:
            prpd_curve_data_read_process(&info);
            break;
        case MSG_READ_PD:
            pd_event_process(&info);
            break;
        case MSG_READ_OV:
            ov_event_process(&info);
            break;
        default:
            LOG_WARN("Curve data received read notify for msg type: %d", msg.type);
            break;
        }
    }

    return 0;
}

kernel_pid_t data_acquisition_service_pid;
char data_acquisition_service_thread_stack[THREAD_STACKSIZE_MAIN * 14] __attribute__((section(".big_data")));
void data_acquisition_service_init(void)
{
    data_acquisition_service_pid = thread_create(data_acquisition_service_thread_stack,
         sizeof(data_acquisition_service_thread_stack),
		 DATA_ACQUISITION_THREAD_PRIORITY,
         THREAD_CREATE_STACKTEST, data_acquisition_service, NULL,
         "acquisition_srv");
    (void) data_acquisition_service_pid;
}


