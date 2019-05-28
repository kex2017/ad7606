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
	uint32_t data_interval;
    double longitude;
    double latitude;
    double height;
	uint32_t channel_threshold[MAX_CHANNEL];
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
void cfg_set_device_data_interval(uint32_t data_interval)
{
	g_device_cfg.device_cfg.data_interval = data_interval;
}

uint32_t cfg_get_device_data_interval(void)
{
    return g_device_cfg.device_cfg.data_interval;
}

/***channel threshold******/
void cfg_set_device_channel_threshold(uint8_t channel, uint32_t threshold)
{
	g_device_cfg.device_cfg.channel_threshold[channel] = threshold;
}

uint32_t cfg_get_device_channel_threshold(uint8_t channel)
{
	return g_device_cfg.device_cfg.channel_threshold[channel];
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


#define DEFAULT_THRESHOLD (100000000U)
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
	}

	memset((void*)&g_device_cfg, 0x0, FLASH_PAGE_SIZE);

	page = get_device_cfg_flash_page_addr();
	flashpage_read(page, g_device_cfg.env_buf);

	if (g_device_cfg.device_cfg.flag != FLAG_OFF) {
		g_device_cfg.device_cfg = device_cfg;
	}
}

void update_device_cfg(void)
{
	uint32_t page = 0;

	g_device_cfg.device_cfg.flag = FLAG_ON;
	page = get_device_cfg_flash_page_addr();
    flashpage_write(page, g_device_cfg.env_buf);
}

void display_device_cfg(void)
{
	printf("\r\nFault Partition device configuration:\r\n");
	printf("\tdevice_id: %d\r\n", g_device_cfg.device_cfg.device_id);
	printf("\tversion: %s\r\n", g_device_cfg.device_cfg.version);
	printf("\tinterval: %lds\r\n", g_device_cfg.device_cfg.data_interval);
	printf("\tthreshold0: %ld\r\n", g_device_cfg.device_cfg.channel_threshold[0]);
	printf("\tthreshold1: %ld\r\n", g_device_cfg.device_cfg.channel_threshold[1]);
	printf("\tthreshold2: %ld\r\n", g_device_cfg.device_cfg.channel_threshold[2]);
	printf("\tthreshold3: %ld\r\n", g_device_cfg.device_cfg.channel_threshold[3]);
	printf("\t******************************************************\r\n");
}

int printenv_command(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	display_device_cfg();
	return 0;
}

