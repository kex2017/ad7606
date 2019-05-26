#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cfg.h"
#include "data_storage.h"
#include "frame_common.h"
#include "frame_encode.h"

enum{
    FLAG_ON = 1,
    FLAG_OFF = -1
};

typedef struct _device_cfg {
    uint8_t flag;
    uint16_t device_id;
    uint32_t uart2_bd;
    double longitude;
    double latitude;
    double height;
    uint8_t version[9];
}device_cfg_t;

static device_cfg_t device_cfg = {
		.flag = FLAG_OFF,
		.device_id = 1001,
		.uart2_bd = 38400,
		.longitude = 120.33,
		.latitude = 30.33,
		.height = 12.7,
		.version = "kl_1.0.0" };

void cfg_set_device_id(uint16_t id)
{
    device_cfg.device_id = id;
}

uint16_t cfg_get_device_id(void)
{
    return device_cfg.device_id;
}


void cfg_set_device_uart2_bd(uint32_t baudrate)
{
    device_cfg.uart2_bd = baudrate;
}

uint32_t cfg_get_device_uart2_bd(void)
{
    return device_cfg.uart2_bd;
}

void cfg_set_device_version(const char *version)
{
    strncpy((char*)device_cfg.version,version,sizeof(version));
}

void cfg_set_device_longitude(double longitude)
{
    device_cfg.longitude = longitude;
}

double cfg_get_device_longitude(void)
{
    return device_cfg.longitude;
}

void cfg_set_device_latitude(double latitude)
{
    device_cfg.latitude = latitude;
}

double cfg_get_device_latitude(void)
{
    return device_cfg.latitude;
}

void cfg_set_device_height(double height)
{
    device_cfg.height = height;
}

double cfg_get_device_height(void)
{
    return device_cfg.height;
}

void load_device_cfg(void)
{
    device_cfg_t cfg = { 0, 0, 0, 0, 0, 0, {0}};

    get_device_cnf((uint8_t *)&cfg, sizeof(device_cfg_t));
    if (FLAG_ON == cfg.flag) {
        memcpy((uint8_t *)&device_cfg, (uint8_t *)&cfg, sizeof(device_cfg_t));
    }

}

void update_device_cfg(void)
{
    device_cfg.flag = FLAG_ON;
    save_device_cnf((uint8_t *)&device_cfg, sizeof(device_cfg_t));
}

uint8_t g_version_buf[200] = {0};
void print_device_cfg(void)
{
	get_version_info(g_version_buf);
	printf("id=%d\r\n"
          "baudrate=%d\r\n"
          "longitude=%f\r\n"
          "latitude=%f\r\n"
          "height=%f\r\n"
		   "version=%s\n",
          device_cfg.device_id, (int)device_cfg.uart2_bd, device_cfg.longitude, device_cfg.latitude, device_cfg.height,g_version_buf);
}
