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
    FLAG_ON = 0x212102,
    FLAG_OFF = 0x121201
};

typedef struct _device_cfg {
	int32_t flag;
	uint16_t device_id;
	uint8_t version[32];
	uint16_t data_interval;
    double longitude;
    double latitude;
    double height;
    uint16_t change_rate[MAX_PHASE][MAX_CHANNEL];
    uint16_t high_change_rate[MAX_PHASE][MAX_CHANNEL];

    uint16_t channel_threshold[MAX_PHASE][MAX_CHANNEL];
    uint16_t high_channel_threshold[MAX_PHASE][MAX_CHANNEL];

	calibration_data_t  calibration_info[MAX_PHASE][MAX_CHANNEL];
	calibration_data_t  high_calibration_info[MAX_PHASE][MAX_CHANNEL];
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
    update_device_cfg();
}

uint16_t* cfg_get_device_data_interval(void)
{
    return &g_device_cfg.device_cfg.data_interval;
}

uint16_t cfg_get_device_pf_channel_threshold(uint8_t phase, uint8_t channel)
{
	return g_device_cfg.device_cfg.channel_threshold[phase][channel];
}

uint16_t cfg_get_device_hf_channel_threshold(uint8_t phase, uint8_t channel)
{
	return g_device_cfg.device_cfg.high_channel_threshold[phase][channel];
}

uint16_t cfg_get_device_pf_channel_changerate(uint8_t phase, uint8_t channel)
{
	return g_device_cfg.device_cfg.change_rate[phase][channel];
}

uint16_t cfg_get_device_hf_channel_changerate(uint8_t phase, uint8_t channel)
{
	return g_device_cfg.device_cfg.high_change_rate[phase][channel];
}

calibration_data_t * cfg_get_calibration_k_b(uint8_t phase, uint8_t channel)
{
	return &g_device_cfg.device_cfg.calibration_info[phase][channel];
}

calibration_data_t * cfg_get_high_calibration_k_b(uint8_t phase, uint8_t channel)
{
	return &g_device_cfg.device_cfg.high_calibration_info[phase][channel];
}

void cfg_set_device_longitude(double longitude)
{
    g_device_cfg.device_cfg.longitude = longitude;
    update_device_cfg();
}

double cfg_get_device_longitude(void)
{
    return g_device_cfg.device_cfg.longitude;
}

void cfg_set_device_latitude(double latitude)
{
    g_device_cfg.device_cfg.latitude = latitude;
    update_device_cfg();
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
    update_device_cfg();
}

void cfg_set_device_threshold(uint8_t phase, uint8_t channel, uint16_t threshold)
{
    g_device_cfg.device_cfg.channel_threshold[phase][channel] = threshold;
    update_device_cfg();
}

void cfg_set_device_changerate(uint8_t phase, uint8_t channel, uint16_t changerate)
{
    g_device_cfg.device_cfg.change_rate[phase][channel] = changerate;
    update_device_cfg();
}

void cfg_set_device_k_b(uint8_t phase, uint8_t channel, float k, float b)
{
	g_device_cfg.device_cfg.calibration_info[phase][channel].k = k;
	g_device_cfg.device_cfg.calibration_info[phase][channel].b = b;
    update_device_cfg();
}


void cfg_set_high_device_threshold(uint8_t phase, uint8_t channel, uint16_t threshold)
{
    g_device_cfg.device_cfg.high_channel_threshold[phase][channel] = threshold;
    update_device_cfg();
}

void cfg_set_high_device_changerate(uint8_t phase, uint8_t channel, uint16_t changerate)
{
    g_device_cfg.device_cfg.high_change_rate[phase][channel] = changerate;
    update_device_cfg();
}

void cfg_set_high_device_k_b(uint8_t phase, uint8_t channel, float k, float b)
{
	g_device_cfg.device_cfg. high_calibration_info[phase][channel].k = k;
	g_device_cfg.device_cfg. high_calibration_info[phase][channel].b = b;
    update_device_cfg();
}

#define DEFAULT_THRESHOLD (200)
#define DATA_CHANGE_RATE (4095U);
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

	for(uint8_t phase = 0; phase < MAX_PHASE; phase++){
        for (i = 0; i < MAX_CHANNEL; i++) {
            device_cfg.channel_threshold[phase][i] = 40000;
            device_cfg.change_rate[phase][i] = DATA_CHANGE_RATE;
            device_cfg.calibration_info[phase][i].k = 1;
            device_cfg.calibration_info[phase][i].b = 0;

            device_cfg.high_channel_threshold[phase][i] = DEFAULT_THRESHOLD;
            device_cfg.high_change_rate[phase][i] = DATA_CHANGE_RATE;
            device_cfg.high_calibration_info[phase][i].k = 1;
            device_cfg.high_calibration_info[phase][i].b = 0;
        }
	}

	memset((void*)&g_device_cfg, 0x0, FLASH_PAGE_SIZE);

	page = get_device_cfg_flash_page_addr();
	flashpage_read(page, g_device_cfg.env_buf);

	cfg_set_device_version(GIT_VERSION);
	if (g_device_cfg.device_cfg.flag != FLAG_ON) {
		g_device_cfg.device_cfg = device_cfg;
		update_device_cfg();
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
	printf("\r\n******************************************************\r\n");
	printf("Fault Partition device configuration:\r\n");
	printf("cfg flag :%s\r\n", (g_device_cfg.device_cfg.flag == FLAG_ON)?"FLAG_ON":"FLAG_OFF");
	printf("\tdevice_id: %d\r\n", g_device_cfg.device_cfg.device_id);
	printf("\tversion: %s\r\n", g_device_cfg.device_cfg.version);
	printf("\tinterval: %ds\r\n", g_device_cfg.device_cfg.data_interval);
	printf("\tpf threshold0: %d\r\n", g_device_cfg.device_cfg.channel_threshold[0][0]);
	printf("\tpf threshold1: %d\r\n", g_device_cfg.device_cfg.channel_threshold[0][1]);
	printf("\thf threshold0: %d\r\n", g_device_cfg.device_cfg.high_channel_threshold[0][0]);
	printf("\thf threshold1: %d\r\n", g_device_cfg.device_cfg.high_channel_threshold[0][1]);
	printf("\tpf change_rete0: %d\r\n", g_device_cfg.device_cfg.change_rate[0][0]);
	printf("\tpf change_rete1: %d\r\n", g_device_cfg.device_cfg.change_rate[0][1]);
	printf("\thf change_rete0: %d\r\n", g_device_cfg.device_cfg.high_change_rate[0][0]);
	printf("\thf change_rete1: %d\r\n", g_device_cfg.device_cfg.high_change_rate[0][1]);
	printf("******************************************************\r\n");
}

int printenv_command(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	display_device_cfg();
	return 0;
}

