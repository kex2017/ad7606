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

#include "log.h"
#include "type_alias.h"
#include "upgrade_from_flash.h"
#include "klfpga.h"
#include "timex.h"
#include "xtimer.h"

#include "fatfs/ff.h"
#include "hashes/md5.h"

#define ENV_DEVICE_START_BASE_ADDR    0x08078800  //34k
#define ENV_START_BASE_ADDR           0x08078000  //32k
#define PROGRAM_FPGA_BASE_ADDR        0x08050000  //160k
#define PROGRAM_UPGRADE_BASE_ADDR     0x08030000  //128k
#define PROGRAM_ACTION_BASE_ADDR      0x08010000  //128k
#define CPU_FLASH_BASE_ADDR           0x08000000  //512k
#define FLAG_CLEAR    0x12121200
#define FLAG_SET      0x21212100

const char *g_std_spi_fpga = "GZDW.bin";
static uint32_t g_last_write_addr = 0x0;
uint32_t g_file_len = 0;
static uint32_t g_cur_index = 0;
static uint8_t g_flash_buf[FLASH_PAGE_SIZE] = { 0 };


typedef struct {
   uint32_t upgrade_flag;
   uint32_t program_len;
   uint8_t program_md5sum[32];
} env_t;

typedef union {
   uint8_t env_buf[FLASH_PAGE_SIZE];
   env_t env;
} g_env_cfg;

g_env_cfg g_env;


static void delay_s(int s)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
}

/************************************env cfg*********************************************/
static void env_save_cfg(void)
{
    int page = 0;
    page = (ENV_START_BASE_ADDR - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    flashpage_write(page, g_env.env_buf);
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
   memcpy(g_env.env.program_md5sum, md5sum, 32);
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

    LOG_INFO("Clean flash info for upgrade file type:%d", type);
}

/**
 * @brief compare received md5sum with calculated file stream md5sum
 *
 * return   1 equal; else unequal
 * */
uint8_t flash_integrity_check(uint8_t file_type, uint8_t *recv_md5)
{
    char digest[16] = { 0 };
    char calc_md5[33] = { 0 };
    int i = 0;

    uint32_t addr = get_flash_start_addr(file_type);

    md5(digest, (void*)addr, g_file_len);
    for (i = 0; i < 16; i++) {
        sprintf(calc_md5 + i * 2, "%02x", digest[i]);
    }

    if (memcmp(calc_md5, recv_md5, 32)) {
        LOG_ERROR("Md5sum received(?) != calculated(%s), file transmission failed.", calc_md5);
        return MD5ERR;
    }

    env_set_program_len(g_file_len);
    env_set_program_md5sum(recv_md5);
    env_save_cfg();

    return DEVICEOK;
}

uint8_t flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint8_t last_pkt_flag)
{
    uint32_t start_addr = 0;
    uint8_t result = SD_FALSE;
    uint16_t left_len = 0;
    int page = 0;

    start_addr = g_last_write_addr;
    page = (start_addr - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    LOG_DEBUG("page:%d  addr:　%d  len: %d  flag: %d  index: %d", page, g_last_write_addr, len, last_pkt_flag,
              g_cur_index);

    if (g_cur_index + len >= FLASH_PAGE_SIZE) {
        memcpy(g_flash_buf + g_cur_index, file_data, FLASH_PAGE_SIZE - g_cur_index);
        left_len = len - (FLASH_PAGE_SIZE - g_cur_index); //FIXME: this length <  FLASH_PAGE_SIZE? have to make sure that!!
        flashpage_write(page, g_flash_buf);

        g_last_write_addr += FLASH_PAGE_SIZE;
        memset(g_flash_buf, 0, FLASH_PAGE_SIZE);
        memcpy(g_flash_buf, file_data + FLASH_PAGE_SIZE - g_cur_index, left_len);
        g_cur_index = left_len;
        result = SD_TRUE;
    }
    else if (g_cur_index + len < FLASH_PAGE_SIZE) {
        memcpy(g_flash_buf + g_cur_index, file_data, len);
        g_cur_index += len;
        if (SD_TRUE != last_pkt_flag) {
            result = SD_TRUE;
        }
        else {
            flashpage_write(page, g_flash_buf);
            g_last_write_addr += FLASH_PAGE_SIZE;
            memset(g_flash_buf, 0, FLASH_PAGE_SIZE);
            g_cur_index = 0;
            result = SD_TRUE;
        }
    }

    if (result == SD_TRUE) {
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
        LOG_INFO("Try to reboot and load program.");
        soft_reset();
    }
    else {
        Config_FPGA(g_file_len, (unsigned char *)get_fpga_start_flash_addr(), 1);
        LOG_INFO("Reconfig FPGA image result: %s.", !is_fpga_microcode_work_no_ok() ? "OK!" : "NOK!");
        soft_reset();
    }
}


void download_fpga_image(uint8_t fpga_no)
{
    FRESULT result;
    FATFS fs;
    FIL file;
    DIR DirInf;
    uint32_t bw;
    unsigned char buf[512];
    int i;
    char *f_name = NULL;

    printf("start download fpga %s image\r\n", (fpga_no==0)?"A":(fpga_no==1)?"B":"C");
    set_cur_ctr_fpga_no(fpga_no);
    f_name = (char*) g_std_spi_fpga;
    /* 挂载文件系统 */
    result = f_mount(&fs, "", 1); /* Mount a logical drive */
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
    }
    /* 打开根文件夹 */
    result = f_opendir(&DirInf, "0:/"); /* 如果不带参数，则从当前目录开始 */
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
        return;
    } else {
        LOG_DEBUG("Root Directory is Open OK");
    }

    /* 打开文件 */
    result = f_open(&file, f_name, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File: %s", f_name);
        return;
    } else {
        LOG_INFO("Open File OK.");
    }
    /* 读取文件 */
//static int cur = 0;
    for (i = 0;; i++) {
        result = f_read(&file, &buf, sizeof(buf), (UINT*) &bw);
        if (result || bw == 0) {
            LOG_INFO("read data from file ok!\r\n");
//            LOG_ERROR("Error: %d:%ld", result, bw);
            break;
        } else {
//            printf("cur download place is %ld num is %d\r\n", bw, ++cur);
            // !i == 1, first flag is true, otherwise, false.
            if (Config_FPGA(bw, buf, !i)) {
                LOG_ERROR("Download PFGA FAIL %d", i);
                break;
            }
        }
    }

    /* 关闭文件*/
    f_close(&file);

    /* 卸载文件系统 */
    f_mount(0, "", 1);

    LOG_INFO("config FPGA %s image result: %s.\r\n", (fpga_no==0)?"A":(fpga_no==1)?"B":"C", !is_fpga_microcode_work_no_ok() ? "OK!" : "NOK!");
}

