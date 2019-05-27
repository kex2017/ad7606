/*
 * upgrade_from_flash.c
 *
 *  Created on: Jan 5, 2018
 *      Author: chenzy
 */

#include <string.h>
#include "periph/flashpage.h"
#include "hashes/md5.h"
#include "periph/pm.h"

#include "type_alias.h"
#include "upgrade_from_flash.h"
#include "klfpga.h"
#include "timex.h"
#include "xtimer.h"
#include "log.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define ENV_DEVICE_START_BASE_ADDR    0x08078800  //34k  mjc:该宏定义程序中未使用
#define ENV_START_BASE_ADDR           0x08078000  //32k
#define PROGRAM_FPGA_BASE_ADDR        0x08050000  //160k
#define PROGRAM_UPGRADE_BASE_ADDR     0x08030000  //128k
#define PROGRAM_ACTION_BASE_ADDR      0x08010000  //128k
#define CPU_FLASH_BASE_ADDR           0x08000000  //512k
#define FLAG_CLEAR    0x12121200
#define FLAG_SET      0x21212100


static uint32_t g_last_write_addr = 0x0;
uint32_t g_file_len = 0;
static uint32_t g_cur_index = 0;
static uint8_t g_flash_buf[FLASH_PAGE_SIZE] = { 0 };


typedef struct {
   uint32_t upgrade_flag;
   uint32_t program_len;
   uint8_t program_md5sum[16];
} env_t;

typedef union {
   uint8_t env_buf[FLASH_PAGE_SIZE];
   env_t env;
} g_env_cfg;

g_env_cfg g_env;


/************************************env cfg*********************************************/

static void delay_s(int s)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
}

uint8_t buff[FLASH_PAGE_SIZE];
static void env_save_cfg(void)
{
    int page = 0;
    page = (ENV_START_BASE_ADDR - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;

    flashpage_write(page, g_env.env_buf);
    flashpage_read(page,buff);
}

void env_set_program_upgrade_flag(uint32_t flag)
{
   g_env.env.upgrade_flag = flag;
}

void env_set_program_len(uint32_t len)
{
   g_env.env.program_len = len;
}

void env_set_program_md5sum(uint8_t* md5sum)
{
   memcpy(g_env.env.program_md5sum, md5sum, 16);
}

uint32_t get_app_start_flash_addr(void)
{
    return PROGRAM_UPGRADE_BASE_ADDR;
}

uint32_t get_device_cfg_flash_page_addr(void)
{
    return (ENV_DEVICE_START_BASE_ADDR - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
}

uint32_t get_fpga_start_flash_addr(void)
{
    return PROGRAM_FPGA_BASE_ADDR;
}

uint32_t get_flash_start_addr(uint8_t type)
{
    uint32_t addr;

    if (FPGA == type) {
        addr = get_fpga_start_flash_addr();
    }
    if (ARM == type) {
        addr = get_app_start_flash_addr();
    }

    return addr;
}

void mock_upgrade_file_len(uint32_t len)
{
	g_file_len = len;
}

void flash_upload_file_handler_init(uint8_t type)
{
    g_file_len = 0;

    g_last_write_addr = get_flash_start_addr(type);

    memset(g_flash_buf, 0x0, FLASH_PAGE_SIZE);
    g_cur_index = 0;

    LOG_INFO("[Upgrade]:Clean flash info for upgrade file type:%d", type);
}

/**
 * @brief compare received md5sum with calculated file stream md5sum
 *
 * return   1 equal; else unequal
 * */
uint8_t flash_integrity_check(uint8_t file_type, uint8_t *recv_md5)
{
	uint8_t digest[16] = { 0 };
//    char calc_md5[33] = { 0 };
    int i = 0;


    uint32_t addr = get_flash_start_addr(file_type);
    md5(digest, (void*)addr, g_file_len);
    (void)recv_md5;


    DEBUG("[Upgrade]: calculated: ");
	for (i = 0; i < 16; i++) {
		DEBUG("%x ",digest[i]);
	}


    env_set_program_len(g_file_len);
    env_set_program_md5sum((uint8_t*)digest);
    env_save_cfg();

    return DEVICEOK;
}

#define SINGLE_PKT_FILE_CONTENT_LEN (256)
uint8_t hardcode_addr_flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag)
{
    int page = 0;
    uint32_t start_addr = 0;

    (void)last_pkt_flag;

    start_addr = get_flash_start_addr(ARM) + file_index * SINGLE_PKT_FILE_CONTENT_LEN;
    page = (start_addr - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    start_addr = CPU_FLASH_BASE_ADDR + page * FLASH_PAGE_SIZE;

    memset((void*)g_flash_buf, 0x0, FLASH_PAGE_SIZE);
    memcpy(g_flash_buf, (void*)start_addr, FLASH_PAGE_SIZE);
    memcpy(g_flash_buf + SINGLE_PKT_FILE_CONTENT_LEN * (file_index % 8), file_data, len);

    flashpage_write(page, g_flash_buf);
    g_file_len += len;
    return 0;
}
uint8_t flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag)
{
    uint32_t start_addr = 0;
    uint8_t result = 1;
    uint16_t left_len = 0;
    int page = 0;

    (void)file_index;

    start_addr = g_last_write_addr;
    page = (start_addr - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    LOG_INFO("[Upgrade]:page:%d  addr:　%ld  len: %d  flag: %d  index: %ld\r\n", page, g_last_write_addr, len, last_pkt_flag,
              g_cur_index);

    if (g_cur_index + len >= FLASH_PAGE_SIZE) {
        memcpy(g_flash_buf + g_cur_index, file_data, FLASH_PAGE_SIZE - g_cur_index);
        left_len = len - (FLASH_PAGE_SIZE - g_cur_index); //FIXME: this length <  FLASH_PAGE_SIZE? have to make sure that!!
        flashpage_write(page, g_flash_buf);

        g_last_write_addr += FLASH_PAGE_SIZE;
        memset(g_flash_buf, 0, FLASH_PAGE_SIZE);
        memcpy(g_flash_buf, file_data + FLASH_PAGE_SIZE - g_cur_index, left_len);
        g_cur_index = left_len;
        result = 0;
    }
    else if (g_cur_index + len < FLASH_PAGE_SIZE) {
        memcpy(g_flash_buf + g_cur_index, file_data, len);
        g_cur_index += len;
        if (0 != last_pkt_flag) {
            result = 0;
        }
        else {
            flashpage_write(page, g_flash_buf);
            g_last_write_addr += FLASH_PAGE_SIZE;
            memset(g_flash_buf, 0, FLASH_PAGE_SIZE);
            g_cur_index = 0;
            result = 0;
        }
    }

    if (result == 0) {
        g_file_len += len;
    }

    return result;
}

void soft_reset(void)
{
    pm_reboot();
}


void flash_set_app_to_run(uint8_t type)
{
    env_set_program_upgrade_flag(FLAG_SET);
    env_save_cfg();


    if (ARM == type) {
        //send response message first, and then reboot
        delay_s(2);
        LOG_INFO("[Upgrade]:Try to reboot and load program.\r\n");
        soft_reset();
    }
    else {
        Config_FPGA(g_file_len, (unsigned char *)get_fpga_start_flash_addr(), 1);
        LOG_INFO("[Upgrade]:Reconfig FPGA image result: %s.", !is_fpga_microcode_work_no_ok() ? "OK!" : "NOK!");
        soft_reset();
    }
}


