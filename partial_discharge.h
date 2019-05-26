/*
 * partial_discharge.h
 *
 *  Created on: Jan 10, 2018
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_PARTIAL_DISCHARGE_H_
#define PARTS_CABLE_GROUND_NEW_PARTIAL_DISCHARGE_H_

#include <stdint.h>
#include "thread.h"

#define BASE_PD_CHAN (10)
#define MAX_PD_CHAN (13)

typedef struct _partial_discharge_event_info {
    uint16_t happened_flag;		// 1: happened, else, no
    uint16_t sn;				// serial number of event
    uint32_t data_len;			// pd curve data length
    uint32_t second;			// happened timestamp, seconds
    uint32_t nanosecond;		// happened timestamp, nanoseconds
    float amplitude;			// pd amplitude
} partial_discharge_event_info_t;

/**
 * @brief init partial discharge thread
 * */
kernel_pid_t partial_discharge_service_init(void);

/**
 * @brief	set channel cfg of threshold and change rate by index
 *
 * @param[in] channel_index		partial discharge channel index: from 0 to 2
 * @param[in] threshold			partial discharge channel threshold
 * @param[in] change_rate		partial discharge channel change rate
 *
 * @return						0: success; else: failure
 * */
uint32_t partial_discharge_set_channel_cfg_by_index(uint8_t channel_index, uint16_t threshold, uint16_t change_rate);

/***
 * @brief	detected partial discharge event, send message from IRQ to partial discharge thread
 * */
int partial_discharge_event_happen_notify(void);

/**
 * @brief	set FPGA sample partial discharge channel configuration
 *
 * @param[in] channel_count		total channel count to set
 * @param[in] cfg				configuration value to set
 *
 * @return						0: success; else: failure
 * */
uint32_t partial_discharge_set_channel_cfg(uint8_t channel_count, void *cfg);


/**
 * @brief	query partial discharge event info between start_time and end_time
 *
 * @param[in]  channel			channel Number which to read, from 0~2
 * @param[out] event_info		pointer to partial_discharge_event_info_t about where event info saved
 *
 * @return						0: success; else: failure
 * */
uint32_t partial_discharge_query_event_info(uint8_t channel, partial_discharge_event_info_t *event_info);

/**
 * @brief   get partial discharge original sn
 *
 * @param[in]  channel        channel Number which to read, from 0~2
 * @param[in]  timestamp
 * @param[in]  nanosecond
 * @param[out] sn             serial number of event count
 *
 * @return                      0: success; else: failure
 * */
int32_t partial_discharge_get_sn(uint8_t channel, uint32_t timestamp, uint32_t nanosecond, uint16_t *sn);

/**
 * @brief	get partial discharge original data length
 *
 * @param[in]  channel			channel Number which to read, from 0~2
 * @param[in]  sn				serial number of event count
 * @param[out] len				data length
 *
 * @return                      0: success; else: failure
 * */
int32_t partial_discharge_get_curve_data_len(uint8_t channel, uint16_t sn, uint32_t *len);


/**
 * @brief	read partial discharge original data with the packet information
 *
 * @param[in] channel           channel Number which to read, from 0~2
 * @param[in] sn				serial number of event count
 * @param[in] size              single packet length
 * @param[in] cun_pkt_num       current packet number
 * @param[out] data             destination to save curve data
 *
 * @return                      ret>0, success; else failure
 **/
int32_t partial_discharge_get_curve_data(uint8_t channel, uint16_t sn, uint32_t size, uint16_t cur_pkt_num, void *data);

int32_t prpd_get_data(uint8_t channel, uint32_t size, uint16_t cur_pkt_num, void *data);
void pd_event_info_channels_init(void);
void partial_discharge_process_event(uint32_t channels, uint32_t timestamp);
void display_pd_event_info(void);
#endif /* PARTS_CABLE_GROUND_NEW_PARTIAL_DISCHARGE_H_ */
