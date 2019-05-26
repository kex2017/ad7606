/*
 * data_storage.c
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#include <string.h>
#include "errno.h"
#include "log.h"

#include "data_storage.h"
#include "spi_flash_storage.h"
#include "type_alias.h"



#define USE_EXT_SRAM_SAVE_CURVE


int data_storage_init(void)
{
   flash_init();
   return 0;
}

#define HF_CURRENT_JF_FLASH_DATA_NUM 4
#define VOLTAGE_WARNING_FLASH_DATA_NUM (4 * 4)
#define DIELECTRIC_LOSS_FLASH_DATA_NUM 6
#define PARTIAL_DOSCHARGE_FLASH_DATA_NUM 6

#define HF_CURRENT_JF_FLASH_DATA_START_ADDR  0
#define VOLTAGE_WARNING_FLASH_DATA_START_ADDR  (MAX_CURVE_DATA_LEN * HF_CURRENT_JF_FLASH_DATA_NUM + HF_CURRENT_JF_FLASH_DATA_START_ADDR)
#define DIELECTRIC_LOSS_FLASH_DATA_START_ADDR  (MAX_CURVE_DATA_LEN * VOLTAGE_WARNING_FLASH_DATA_NUM + VOLTAGE_WARNING_FLASH_DATA_START_ADDR)
#define DEVICE_CNF_FLASH_DATA_START_ADDR       (MAX_CURVE_DATA_LEN * DIELECTRIC_LOSS_FLASH_DATA_NUM + DIELECTRIC_LOSS_FLASH_DATA_START_ADDR)
#define PARTIAL_DISCHARGE_FLASH_DATA_START_ADDR (MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN + DEVICE_CNF_FLASH_DATA_START_ADDR)

#define OVER_VOLTAGE_WARNING_FLASH_DATA_CHANNEL_OFFSET (MAX_CURVE_DATA_LEN * MAX_OV_EVENT_COUNT)


static uint32_t g_common_curve_data_len[10] = {0};
static uint8_t g_saved_common_curve_data[MAX_COMMON_CHANNELS][MAX_CURVE_DATA_LEN] __attribute__((section(".big_data")));
uint32_t get_curve_data_len(uint8_t channel)
{
    return g_common_curve_data_len[channel];
}

void set_curve_data_len(uint8_t channel, uint32_t len)
{
    g_common_curve_data_len[channel] = len;
}

/**************************************************
 * get curve data start address in ext sram
 * ************************************************/
uint32_t get_curve_data_start_addr(uint8_t channel)
{
    return (uint32_t)g_saved_common_curve_data[channel];
}

/**************************************************
 * save hf_and_dl curve_data to ext sram
 * ************************************************/
uint32_t set_curve_data(uint8_t channel, uint8_t *src, uint32_t size)
{
    uint32_t start_addr = 0;
    uint32_t ret = 0;

    start_addr = get_curve_data_start_addr(channel);
    set_curve_data_len(channel, size);
    memcpy((void*)start_addr, src, size);

    return ret;
}

uint32_t get_curve_data(void* dest, uint32_t addr, uint32_t size)
{
    uint32_t ret = 0;

    memcpy(dest, (void*)addr, size);
    return ret;
}

/****************************************************
 * save over voltage curve data
 ****************************************************/
static uint8_t g_ov_saved_curve_data[MAX_OV_CHANNEL_COUNT * MAX_OV_EVENT_COUNT][MAX_CURVE_DATA_LEN] __attribute__((section(".big_data")));
uint32_t get_over_voltage_start_addr(uint8_t channel, uint8_t sn)
{
#if defined USE_EXT_SRAM_SAVE_CURVE
   return (uint32_t)g_ov_saved_curve_data[channel * MAX_PARTIAL_DISCHARGE_EVENT_COUNT + sn];
#else
   if (channel < MAX_OV_CHANNEL_COUNT && sn < MAX_OV_EVENT_COUNT) {
       return VOLTAGE_WARNING_FLASH_DATA_START_ADDR + channel * OVER_VOLTAGE_WARNING_FLASH_DATA_CHANNEL_OFFSET + sn * MAX_CURVE_DATA_LEN;
   }
#endif
}

uint32_t set_over_voltage_curve_data(uint8_t channel, uint8_t sn, uint8_t *src, uint32_t size)
{
    uint32_t ret = 0;

#if defined USE_EXT_SRAM_SAVE_CURVE
    uint32_t addr = get_over_voltage_start_addr(channel, sn);
    memcpy((void*)addr, src, (size_t)size);
#else
    uint32_t start_addr = 0;
    start_addr = get_over_voltage_start_addr(channel, sn);
    ret = flash_write_data(src, start_addr, size);
#endif
    return ret;
}

uint32_t get_over_voltage_curve_data(void* dest, uint32_t addr, uint32_t size)
{
    uint32_t ret = 0;

#if defined USE_EXT_SRAM_SAVE_CURVE
    memcpy(dest, (void*)addr, size);
#else
    ret = flash_read_data(dest, addr, size);
#endif
    return ret;
}

/**************************************************
 * save spi flash for partial discharge original data
 * ************************************************/
static uint8_t g_pd_saved_curve_data[MAX_PARTIAL_DISCHARGE_AND_PRPD_CHANNEL_COUNT * MAX_PARTIAL_DISCHARGE_EVENT_COUNT][MAX_CURVE_DATA_LEN] __attribute__((section(".big_data")));
uint32_t get_partial_discharge_start_addr(uint8_t channel, uint8_t sn)
{
#if defined USE_EXT_SRAM_SAVE_CURVE
	return (uint32_t)g_pd_saved_curve_data[channel * MAX_PARTIAL_DISCHARGE_EVENT_COUNT + sn];
#else
    if (channel < MAX_PARTIAL_DISCHARGE_AND_PRPD_CHANNEL_COUNT && sn < MAX_PARTIAL_DISCHARGE_EVENT_COUNT) {
    	return PARTIAL_DISCHARGE_FLASH_DATA_START_ADDR + (channel * MAX_PARTIAL_DISCHARGE_EVENT_COUNT + sn) * MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN;
    }
#endif
    LOG_WARN("Get invalid partial discharge channel:%d, sn:%d.", channel, sn);
    return -ENOTSUP;
}

uint32_t set_pd_or_prpd_curve_data(uint8_t index, uint8_t sn, void *src, uint32_t size)
{
    uint32_t ret = 0;

#if defined USE_EXT_SRAM_SAVE_CURVE
    uint32_t addr = get_partial_discharge_start_addr(index, sn);
    memcpy((void*)addr, src, (size_t)size);
#else
    uint32_t start_addr = 0;
    start_addr = get_partial_discharge_start_addr(index, sn);
    ret = flash_write_data(src, start_addr, size);
#endif
    return ret;
}

uint32_t get_pd_or_prpd_curve_data(uint32_t addr, uint32_t size, void* dest)
{
    uint32_t ret = 0;

#if defined USE_EXT_SRAM_SAVE_CURVE
    memcpy(dest, (void*)addr, size);
#else
    ret = flash_read_data(dest, addr, size);
#endif
    return ret;
}

static uint32_t get_device_cnf_spi_flash_addr(void)
{
    return DEVICE_CNF_FLASH_DATA_START_ADDR;
}

int save_device_cnf(uint8_t *src, uint32_t size)
{
    int ret = 0;
    uint32_t addr = 0;
    addr = get_device_cnf_spi_flash_addr();
    ret = flash_write_data(src, addr, size);

    return ret;
}

int get_device_cnf(uint8_t *dest, uint32_t size)
{
    int ret = 0;
    uint32_t addr = 0;
    addr = get_device_cnf_spi_flash_addr();
    ret = flash_read_data(dest, addr, size);

    return ret;
}
