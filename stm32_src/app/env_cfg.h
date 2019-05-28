/*
 * env_cfg.h
 *
 *  Created on: Mar 19, 2018
 *      Author: chenzy
 */

#ifndef PARTS_FAULT_PARTITION_ENV_CFG_H_
#define PARTS_FAULT_PARTITION_ENV_CFG_H_

#include <stdint.h>


void cfg_set_device_id(uint16_t id);
uint16_t cfg_get_device_id(void);

void cfg_set_device_version(char *version);
char* cfg_get_device_version(void);

void cfg_set_device_data_interval(uint32_t data_interval);
uint32_t cfg_get_device_data_interval(void);

void cfg_set_device_channel_threshold(uint8_t channel, uint32_t threshold);
uint32_t cfg_get_device_channel_threshold(uint8_t channel);


void cfg_set_device_longitude(double longitude);
double cfg_get_device_longitude(void);
void cfg_set_device_latitude(double latitude);
double cfg_get_device_latitude(void);
void cfg_set_device_height(double height);
double cfg_get_device_height(void);

void load_device_cfg(void);
void update_device_cfg(void);
void display_device_cfg(void);
int printenv_command(int argc, char **argv);

#endif /* PARTS_FAULT_PARTITION_ENV_CFG_H_ */
