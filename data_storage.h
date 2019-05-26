/*
 * data_storage.h
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_DATA_STORAGE_H_
#define PARTS_CABLE_GROUND_NEW_DATA_STORAGE_H_

#include <stdint.h>

#define MAX_CURVE_DATA_LEN (20480 + 4096) //24k

#define MAX_PARTIAL_DISCHARGE_AND_PRPD_CHANNEL_COUNT	6
#define MAX_PARTIAL_DISCHARGE_EVENT_COUNT	1
#define MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT    3

#define MAX_COMMON_CHANNELS (10)

#define MAX_OV_CHANNEL_COUNT  3
#define MAX_OV_EVENT_COUNT 1

int data_storage_init(void);

uint32_t get_curve_data_start_addr(uint8_t channel);
uint32_t set_curve_data(uint8_t channel, uint8_t *src, uint32_t size);
uint32_t get_curve_data(void* dest, uint32_t addr, uint32_t size);


uint32_t get_over_voltage_start_addr(uint8_t channel, uint8_t sn);
uint32_t set_over_voltage_curve_data(uint8_t channel, uint8_t sn, uint8_t *src, uint32_t size);
uint32_t get_over_voltage_curve_data(void* dest, uint32_t addr, uint32_t size);

uint32_t get_partial_discharge_start_addr(uint8_t channel, uint8_t sn);
uint32_t set_pd_or_prpd_curve_data(uint8_t index, uint8_t sn, void* src, uint32_t size);
uint32_t get_pd_or_prpd_curve_data(uint32_t addr, uint32_t size, void* dest);


void set_curve_data_len(uint8_t channel, uint32_t len);
uint32_t get_curve_data_len(uint8_t channel);

int get_device_cnf(uint8_t *dest, uint32_t size);
int save_device_cnf(uint8_t *src, uint32_t size);

#endif /* PARTS_CABLE_GROUND_NEW_DATA_STORAGE_H_ */
