/*
 * partial_discharge.c
 *
 *  Created on: Jan 10, 2018
 *      Author: chenzy
 */

#include <string.h>

#include "msg.h"
#include "periph/rtt.h"
#include "board.h"

#include "log.h"
#include "type_alias.h"

#include "partial_discharge.h"
#include "data_storage.h"
#include "daq.h"
#include "circular_task.h"
#include "x_delay.h"
#include "cable_ground_threads.h"
#include "upgrade_from_sd_card.h"



/**
 * @brief   PID of the over voltage thread
 */
kernel_pid_t partial_discharge_pid = KERNEL_PID_UNDEF;


static uint8_t g_pd_channel_index[MAX_PARTIAL_DISCHARGE_AND_PRPD_CHANNEL_COUNT] = {10, 11, 12, 13, 14, 15};//mapping to 0, 1, 2
static partial_discharge_event_info_t g_channel_pd_event_info[MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT][MAX_PARTIAL_DISCHARGE_EVENT_COUNT];
static uint32_t g_pd_event_count[MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT] = {0};

const uint8_t magic_number = 99; //this magic number add one must be power of MAX_PARTIAL_DISCHARGE_EVENT_COUNT!
void pd_event_info_channels_init(void)
{
	uint8_t index = 0;

	memset(g_channel_pd_event_info, 0x0, sizeof(g_channel_pd_event_info));
	memset(g_pd_event_count, 0x0, sizeof(g_pd_event_count));

	for (index = 0; index < MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT; index++) {
		g_pd_event_count[index] = magic_number;
	}
}

void update_pd_event_count_info(uint8_t index)
{
	if (magic_number <= g_pd_event_count[index]) {
		LOG_WARN("Get partial discharge count as(%d) > (%d), and it will overflow soon, reset to 0 now.", g_pd_event_count[index], magic_number);
		g_pd_event_count[index] = 0;
		return;
	}
	g_pd_event_count[index]++;
}

uint16_t get_pd_event_count_index(uint8_t index)
{
	return g_pd_event_count[index] % MAX_PARTIAL_DISCHARGE_EVENT_COUNT;
}

partial_discharge_event_info_t* get_channel_partial_discharge_event_info(uint8_t channel)
{
    return g_channel_pd_event_info[channel];
}

partial_discharge_event_info_t* get_partial_discharge_event_info(uint8_t channel, uint16_t sn)
{
    return &(g_channel_pd_event_info[channel][sn]);
}

void display_pd_event_info(void)
{
    for(int channel = 0; channel < 3; channel++){
        LOG_INFO("amplitude = %f data_len = %u happened_flag = %u",
        g_channel_pd_event_info[channel][0].amplitude,
        g_channel_pd_event_info[channel][0].data_len,
        g_channel_pd_event_info[channel][0].happened_flag
        );
    }
}

void update_channel_pd_event_info(uint8_t index, uint32_t timestamp, uint32_t len, float amplitude, uint32_t ns)
{
	partial_discharge_event_info_t *event_info = NULL;

	if (index >= MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT) {
		LOG_WARN("Get partial discharge channel index(%d) >= expected(%d), dropped!", index, MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT);
		return;
	}
	update_pd_event_count_info(index);

	event_info = get_partial_discharge_event_info(index, get_pd_event_count_index(index));
	event_info->happened_flag = 1;
	event_info->sn = get_pd_event_count_index(index);
	event_info->data_len = len;
	event_info->second = timestamp;
	event_info->nanosecond = ns;
	event_info->amplitude = amplitude;
}

float read_partial_dischatge_amplitude(void)
{
	//FIXME: add API in DAQ module
	return 1.0;
}

static uint8_t g_pd_data[MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN] __attribute__((section(".big_data")));
void read_and_save_partial_discharge_data(uint8_t channel)
{
    uint32_t length = 0, i;
    uint32_t ns = 0;
    float amplitude;
    uint32_t timestamp;

    if ((length = daq_fsmc_sample_len_get(channel)) > MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN) {
    	LOG_WARN("Get PD length(%lu) > max length, set as max length!", length);
    	length = MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN;
    }

    memset(g_pd_data, 0x0, MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN);
    daq_fsmc_sample_data_read(channel, g_pd_data, 0, length);
    set_pd_or_prpd_curve_data(channel - BASE_PD_CHAN, get_pd_event_count_index(channel - BASE_PD_CHAN), (void*)g_pd_data, length);
    for (i = 0; i < 200; i++) {
    	LOG_DEBUG("%d-%dth: big-%02x%02x", channel, i, g_pd_data[2*i + 1], g_pd_data[2*i]);
    }
	for (i = length/2 - 200; i < length/2; i++) {
		LOG_DEBUG("%d-%dth: big-%02x%02x", channel, i, g_pd_data[2 * i + 1], g_pd_data[2 * i]);
	}

	amplitude = read_partial_dischatge_amplitude();
	ns = daq_fsmc_pd_ns_get(channel);
	daq_fsmc_pd_seconds_get(&timestamp);//TODO:fpga 读取的时间戳不正确
	update_channel_pd_event_info(channel - BASE_PD_CHAN, timestamp, length, amplitude, ns);
    LOG_INFO("Read PD original data from FPGA, channel:%d, length:%d, timestamp:%ld, ns:%d", channel, length, timestamp, ns);
    if (get_save_curve_data_to_sd_flag()) {
        save_curve_data_to_sd(channel, g_pd_data, length);
    }
}

void reset_all_pd_data_flag(uint16_t channels)
{
    acquire_fsmc_ops_mtx();
    daq_fsmc_pd_flag_reset(channels);
    release_fsmc_ops_mtx();
}

void partial_discharge_process_event(uint32_t channels, uint32_t timestamp)
{
    (void)timestamp;
    uint8_t channel;
    uint8_t reset_flag = 0;

    for (channel = BASE_PD_CHAN; channel < MAX_PD_CHAN; channel++) {
        if (channels & (0x01 << channel)) {
            acquire_fsmc_ops_mtx();
            if (daq_fsmc_sample_done_check(channel) > 0) {
                read_and_save_partial_discharge_data(channel);
                reset_flag = 1;
            }
            else {
                LOG_WARN("Curve data for PD channel:%d is not ready yet.", channel);
            }
            release_fsmc_ops_mtx();
            delay_ms(10);
        }
    }
    //@XieWei: reset three channels' flag in one time
    if (reset_flag) {
        reset_all_pd_data_flag(0x1C00);
        reset_flag = 0;
    }
}


uint32_t partial_discharge_set_channel_cfg_by_index(uint8_t channel_index, uint16_t threshold, uint16_t change_rate)
{
	if (channel_index >= MAX_PARTIAL_DISCHARGE_AND_PRPD_CHANNEL_COUNT) {
		return DEVNOSUP;
	}
	daq_fsmc_channel_pd_cfg_set(g_pd_channel_index[channel_index], threshold, change_rate);

	return DEVICEOK;
}


uint32_t partial_discharge_set_channel_cfg(uint8_t channel_count, void *cfg)
{
	(void)cfg;
	int i = 0;
	const uint16_t DEFAULT_PD_THRESHOLD = 2047, DEFAULT_PD_CHANGE_RATE = 4095;

	for (i = 0; i < channel_count; i++) {
		partial_discharge_set_channel_cfg_by_index(i, DEFAULT_PD_THRESHOLD, DEFAULT_PD_CHANGE_RATE);
	}

	return DEVICEOK;
}

uint32_t partial_discharge_query_event_info(uint8_t channel, partial_discharge_event_info_t *event_info)
{
	uint8_t i;
	partial_discharge_event_info_t *tmp = NULL;
	tmp = (partial_discharge_event_info_t*)event_info;
	for (i = 0; i < MAX_PARTIAL_DISCHARGE_EVENT_COUNT; i++) {
	    *tmp++ = *get_partial_discharge_event_info(channel, i);
	}
	return DEVICEOK;
}



int32_t partial_discharge_get_sn(uint8_t channel, uint32_t timestamp, uint32_t nanosecond, uint16_t *sn)
{
    for (uint8_t i = 0; i < MAX_PARTIAL_DISCHARGE_EVENT_COUNT; i++) {
//        if ((g_channel_pd_event_info[channel][i].second = timestamp)
//                        && (g_channel_pd_event_info[channel][i].nanosecond = nanosecond)) {
            *sn = 0;// g_channel_pd_event_info[channel][i].sn;
            return DEVICEOK;
//        }
    }

    LOG_WARN("No partial discharge event sn for channel:%d, seconds:%ld, nanoseconds:%ld yet!", channel, timestamp, nanosecond);
    return -DEVNODATA;
}

int32_t partial_discharge_get_curve_data_len(uint8_t channel, uint16_t sn, uint32_t *len)
{
	partial_discharge_event_info_t *event_info = NULL;

	event_info = get_partial_discharge_event_info(channel, sn);
	if (event_info->happened_flag) {
		*len = event_info->data_len;
		return DEVICEOK;
	}

	LOG_WARN("No partial discharge event happened for channel:%d, sn:%d yet!", channel, sn);
	return -DEVNODATA;
}

int32_t partial_discharge_get_curve_data(uint8_t channel, uint16_t sn, uint32_t size, uint16_t cur_pkt_num, void *data)
{
	uint32_t ret = DEVICEOK;
	uint32_t start_addr;
	uint16_t total_pkt_count = 0;
	partial_discharge_event_info_t *event_info = NULL;

	event_info = get_partial_discharge_event_info(channel, sn);
	if (!event_info->happened_flag) {
		LOG_WARN("No partial discharge event happened for channel:%d, sn:%d yet!", channel, sn);
		return -DEVNODATA;
	}
	total_pkt_count = event_info->data_len / size + !!(event_info->data_len % size);
	start_addr = get_partial_discharge_start_addr(channel, sn);
	ret = get_pd_or_prpd_curve_data(start_addr + cur_pkt_num * size, size, data);

    if ((total_pkt_count - 1) == cur_pkt_num || 0 == cur_pkt_num) {
    	LOG_INFO("Read partial discharge original data: channel:%d, sn:%d, total_pkt_count:%d, cur_pkt_num:%d.", channel, sn, total_pkt_count, cur_pkt_num);
    }

	return ret;
}

int32_t prpd_get_data(uint8_t channel, uint32_t size, uint16_t cur_pkt_num, void *data)
{
    uint32_t ret = DEVICEOK;
    uint32_t start_addr;
    uint16_t total_pkt_count = 0;
    uint32_t data_len = 0;
    uint32_t logic_channel = channel + 13;

    data_len  = daq_fsmc_sample_len_get(logic_channel);
    total_pkt_count = data_len / size + !!(data_len % size);
    start_addr = get_partial_discharge_start_addr(logic_channel-10, 0);
    ret = get_pd_or_prpd_curve_data(start_addr + cur_pkt_num * size, size, data);

    if ((total_pkt_count - 1) == cur_pkt_num || 0 == cur_pkt_num) {
        LOG_INFO("Read prpd data: channel:%d, total_pkt_count:%d, cur_pkt_num:%d.", logic_channel, total_pkt_count, cur_pkt_num);
    }

    return ret;
}
