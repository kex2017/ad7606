/*
 * env_cfg.c
 *
 *  Created on: Mar 19, 2018
 *      Author: chenzy
 */

#include <stdio.h>
#include <string.h>

#include "type_alias.h"
#include "env_cfg.h"
#include "upgrade_from_flash.h"
#include "frame_handler.h"
#include "periph/flashpage.h"

enum{
    FLAG_ON = 1,
    FLAG_OFF = -1
};

typedef struct _device_cfg {
	int32_t flag;
	uint16_t device_id;
	uint8_t version[32];
	uint16_t data_interval;
    double longitude;
    double latitude;
    double height;
    uint16_t change_rate[MAX_CHANNEL];
    uint16_t high_change_rate[MAX_CHANNEL];

    uint16_t channel_threshold[MAX_CHANNEL];
    uint16_t high_channel_threshold[MAX_CHANNEL];

	calibration_data_t  calibration_info[MAX_CHANNEL];
	calibration_data_t  high_calibration_info[MAX_CHANNEL];
} device_cfg_t;

typedef union {
   uint8_t env_buf[FLASH_PAGE_SIZE];
   device_cfg_t device_cfg;
} env_device_cfg_t;

static env_device_cfg_t g_device_cfg = {0};

/***ID******/
void cfg_set_device_id(uint16_t id)
{
	g_device_cfg.device_cfg.device_id = id;
}

uint16_t cfg_get_device_id(void)
{
    return g_device_cfg.device_cfg.device_id;
}

/***version******/
void cfg_set_device_version(char *version)
{
	memset((char*)g_device_cfg.device_cfg.version, 0x0, sizeof(g_device_cfg.device_cfg.version));
    strncpy((char*)g_device_cfg.device_cfg.version, version, strlen(version));
}

char* cfg_get_device_version(void)
{
    return (char*)g_device_cfg.device_cfg.version;
}

/***data interval******/
void cfg_set_device_data_interval(uint16_t data_interval)
{
	g_device_cfg.device_cfg.data_interval = data_interval;
}

uint16_t cfg_get_device_data_interval(void)
{
    return g_device_cfg.device_cfg.data_interval;
}

uint16_t cfg_get_device_channel_threshold(uint8_t channel)
{
	return g_device_cfg.device_cfg.channel_threshold[channel];
}

uint16_t cfg_get_device_high_channel_threshold(uint8_t channel)
{
	return g_device_cfg.device_cfg.high_channel_threshold[channel];
}

uint16_t cfg_get_device_channel_changerate(uint8_t channel)
{
	return g_device_cfg.device_cfg.change_rate[channel];
}

uint16_t cfg_get_device_high_channel_changerate(uint8_t channel)
{
	return g_device_cfg.device_cfg.high_change_rate[channel];
}

calibration_data_t * cfg_get_calibration_k_b(uint8_t channel)
{
	return &g_device_cfg.device_cfg.calibration_info[channel];
}

calibration_data_t * cfg_get_high_calibration_k_b(uint8_t channel)
{
	return &g_device_cfg.device_cfg.high_calibration_info[channel];
}

void cfg_set_device_longitude(double longitude)
{
    g_device_cfg.device_cfg.longitude = longitude;
}

double cfg_get_device_longitude(void)
{
    return g_device_cfg.device_cfg.longitude;
}

void cfg_set_device_latitude(double latitude)
{
    g_device_cfg.device_cfg.latitude = latitude;
}

double cfg_get_device_latitude(void)
{
    return g_device_cfg.device_cfg.latitude;
}

void cfg_set_device_height(double height)
{
    g_device_cfg.device_cfg.height = height;
}

double cfg_get_device_height(void)
{
    return g_device_cfg.device_cfg.height;
}

void cfg_set_device_threshold(uint8_t channel, uint16_t threshold)
{
    g_device_cfg.device_cfg.channel_threshold[channel] = threshold;
    update_device_cfg();
}

void cfg_set_device_changerate(uint8_t channel, uint16_t changerate)
{
    g_device_cfg.device_cfg.change_rate[channel] = changerate;
    update_device_cfg();
}

void cfg_set_device_k_b(uint8_t channel, float k, float b)
{
	g_device_cfg.device_cfg.calibration_info[channel].k = k;
	g_device_cfg.device_cfg.calibration_info[channel].b = b;
    update_device_cfg();
}


void cfg_set_high_device_threshold(uint8_t channel, uint16_t threshold)
{
    g_device_cfg.device_cfg.high_channel_threshold[channel] = threshold;
    update_device_cfg();
}

void cfg_set_high_device_changerate(uint8_t channel, uint16_t changerate)
{
    g_device_cfg.device_cfg.high_change_rate[channel] = changerate;
    update_device_cfg();
}

void cfg_set_high_device_k_b(uint8_t channel, float k, float b)
{
	g_device_cfg.device_cfg. high_calibration_info[channel].k = k;
	g_device_cfg.device_cfg. high_calibration_info[channel].b = b;
    update_device_cfg();
}

#define DEFAULT_THRESHOLD (10000U)
#define DATA_CHANGE_RATE (200U);
void load_device_cfg(void)
{
	uint32_t page = 0;
	uint32_t i = 0;

	device_cfg_t device_cfg = {
			.flag = FLAG_OFF,
			.device_id = 1001,
			.version = "kl_fl_1.0.0",
			.longitude = 120.33,
            .latitude = 30.33,
            .height = 12.7,
			.data_interval = 60,
	};

	for (i = 0; i < MAX_CHANNEL; i++) {
		device_cfg.channel_threshold[i] = DEFAULT_THRESHOLD;
		device_cfg.change_rate[i] = DATA_CHANGE_RATE;
		device_cfg.calibration_info[i].k = 1;
		device_cfg.calibration_info[i].b = 0;

		device_cfg.high_channel_threshold[i] = DEFAULT_THRESHOLD;
		device_cfg.high_change_rate[i] = DATA_CHANGE_RATE;
		device_cfg.high_calibration_info[i].k = 1;
		device_cfg.high_calibration_info[i].b = 0;
	}


	memset((void*)&g_device_cfg, 0x0, FLASH_PAGE_SIZE);

	page = get_device_cfg_flash_page_addr();
	flashpage_read(page, g_device_cfg.env_buf);

	if (g_device_cfg.device_cfg.flag != FLAG_OFF) {
		g_device_cfg.device_cfg = device_cfg;
//		update_device_cfg();
	}
}

void update_device_cfg(void)
{
	uint32_t page = 0;

	g_device_cfg.device_cfg.flag = FLAG_OFF;
	page = get_device_cfg_flash_page_addr();
    flashpage_write(page, g_device_cfg.env_buf);
}

void display_device_cfg(void)
{
	printf("\r\n******************************************************\r\n");
	printf("Fault Partition device configuration:\r\n");
	printf("\tdevice_id: %d\r\n", g_device_cfg.device_cfg.device_id);
	printf("\tversion: %s\r\n", g_device_cfg.device_cfg.version);
	printf("\tinterval: %ds\r\n", g_device_cfg.device_cfg.data_interval);
	printf("\tthreshold0: %d\r\n", g_device_cfg.device_cfg.channel_threshold[0]);
	printf("\tthreshold1: %d\r\n", g_device_cfg.device_cfg.channel_threshold[1]);
	printf("\thigh_threshold0: %d\r\n", g_device_cfg.device_cfg.high_channel_threshold[0]);
	printf("\thigh_threshold1: %d\r\n", g_device_cfg.device_cfg.high_channel_threshold[1]);
	printf("\tchange_rete0: %d\r\n", g_device_cfg.device_cfg.change_rate[0]);
	printf("\tchange_rete1: %d\r\n", g_device_cfg.device_cfg.change_rate[1]);
	printf("\thigh_change_rete0: %d\r\n", g_device_cfg.device_cfg.high_change_rate[0]);
	printf("\thigh_change_rete1: %d\r\n", g_device_cfg.device_cfg.high_change_rate[1]);
	printf("******************************************************\r\n");
}

int printenv_command(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	display_device_cfg();
	return 0;
}

