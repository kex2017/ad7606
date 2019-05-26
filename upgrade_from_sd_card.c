/*
 * upgrade_from_sd_card.c
 *
 *  Created on: Jan 3, 2018
 *      Author: chenzy
 */

#include <stdio.h>
#include <time.h>
#include "type_alias.h"

#include "log.h"
#include "fatfs/ff.h"
#include "periph/rtt.h"
#include "hashes/md5.h"

#include "klfpga.h"
#include "upgrade_from_sd_card.h"

#include "ext_fpga_img_upgrade.h"

const char *g_tmp_fpga_file = "fpga_tmp";
const char *g_std_fsmc_fpga = "ce30_zip";
const char *g_std_fsmc_fpga_unzip = "ce30_rbf";
const char *g_std_spi_fpga = "fpga_spi.rbf";
static char g_file_buff[1024] __attribute__((section(".big_data")));


static uint8_t save_curve_data_to_sd_flag ;
uint8_t get_save_curve_data_to_sd_flag(void)
{
    return save_curve_data_to_sd_flag;
}

void set_save_curve_data_to_sd_flag(void)
{
    save_curve_data_to_sd_flag = 1;
}

void clear_save_curve_data_to_sd_flag(void)
{
    save_curve_data_to_sd_flag = 0;
}

void save_curve_data_to_sd(uint8_t channel, uint8_t *data_buf, int length)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t bw;

    int i = 0, timestamp;
    uint16_t temp_value = 0;
    char timestamp_buf[30] = { 0 };
    char filename[20] = { 0 };
    char pd_data[10] = { 0 };
    char pd_buf[520] = { 0 };

    timestamp = rtt_get_counter();
    struct tm *t = localtime((const time_t *) &timestamp);
    sprintf(timestamp_buf, "%d-%02d-%02d %02d:%02d:%02d:%d\r\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, length);
    printf("\r\n%d-%02d-%02d %02d:%02d:%02d:start save channel:%d data..\r\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, channel);
    //文件名：channel+mon+day+hour
    sprintf(filename, "%02d%02d%02d%02d", channel, t->tm_mon + 1, t->tm_mday, t->tm_hour);

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return;
    }
    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
    }
    result = f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File: %s", filename);
        return;
    }
    f_write(&file, (void*)timestamp_buf, strlen(timestamp_buf), (UINT*)&bw);
    for (i = 0; i < length / 2; i++) {
        if (i % 100 == 0) {
            thread_yield();
        }
        temp_value = (data_buf[2 * i + 1] << 8 | data_buf[2 * i]);
        sprintf(pd_data, "%5d\t", temp_value);
        strcat(pd_buf, pd_data);
        if (channel > 5 && channel < 12 && (i % 4 == 0)) {
            strcat(pd_buf, "\n");
        }
        if (strlen(pd_buf) > 500) {
            result = f_write(&file, (void*)pd_buf, strlen(pd_buf), (UINT*)&bw);
            if (result != FR_OK) {
                LOG_ERROR("File Write Failed Error(%d)", result);
            }
            memset(pd_buf, 0, sizeof(pd_buf));
        }
    }

    f_close(&file);
    f_mount(0, "", 1);
    printf("--------save channel:%d data ok-------\r\n", channel);

}

//扫描SD卡中的文件
uint8_t scan_sd_card_files(void)
{
	FATFS fs;
	FILINFO fno;
	FRESULT result;
	DIR dir_info;
	int i;
	char *fn;
    char path[20] = {0};

	result = f_mount(&fs, "", 1);
	if (result != FR_OK) {
		LOG_ERROR("FileSystem Mounted Failed (%d)", result);
		return result;
	}

	result = f_opendir(&dir_info, "0:/");
	if (result != FR_OK) {
		LOG_ERROR("Root Directory is Open Error (%d)", result);
		//unmount
		f_mount(0, "", 1);
		return result;
	}
	else {
		i = strlen(path);
		for (;;) {
			result = f_readdir(&dir_info, &fno);
			if (result != FR_OK || fno.fname[0] == 0)
				break;
			if (fno.fname[0] == '.')
				continue;
			fn = fno.fname;
			if (fno.fattrib & AM_DIR) {
				sprintf(&path[i], "/%s", fn);
			} else {
				printf("%s/%s\n", path, fn);
			}
		}
	}

	//unmount
	f_mount(0, "", 1);
	return result;
}


uint8_t sd_card_rename_file(const char *src_file, const char *dest_file)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return result;
    }

    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
        //unmount
        f_mount(0, "", 1);
        return result;
    }

    f_rename(src_file, dest_file);

    //unmount
    f_mount(0, "", 1);

    return result;
}


uint8_t delete_sd_card_file(const char *filename)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return result;
    }

    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
        //unmount
        f_mount(0, "", 1);
        return result;
    }

    f_unlink(filename);

    //unmount
    f_mount(0, "", 1);

    return result;
}

void sd_card_upload_file_handler_init(uint8_t type)
{
    LOG_INFO("Clean SD card tmp file:%s for upgrade file type:%d", g_tmp_fpga_file, type);
    delete_sd_card_file(g_tmp_fpga_file);
}

uint8_t sd_card_calc_file_md5sum(const char *filename, uint8_t *md5sum)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t bw;
    uint32_t i;
    uint8_t md5_buf[16] = {0};
    md5_ctx_t ctx = {0};

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return result;
    }

    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
        //unmount
        f_mount(0, "", 1);
        return result;
    }

    result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File : %s", filename);
        //unmount
        f_mount(0, "", 1);
        return result;
    }

    memset(&ctx, 0, sizeof(md5_ctx_t));
    md5_init(&ctx);
    for (i = 0; ; i++) {
        result = f_read(&file, &g_file_buff, sizeof(g_file_buff), (UINT*)&bw);
        if (result || bw == 0) {
//            printf("Read file Error (%d:%ld)\r\n", result, bw);
            break;
        }
        else {
            LOG_DEBUG("read file len:%ld", bw);
            md5_update(&ctx, g_file_buff, bw);
            memset(g_file_buff, 0, sizeof(g_file_buff));
        }
    }
    md5_final(&ctx, md5_buf);
    LOG_DEBUG("calculate md5sum start.");
    for (i = 0; i< 16; i++) {
        printf("%02x", md5_buf[i]);
        sprintf((char*)(md5sum + i * 2), "%02x", md5_buf[i]);
    }
    printf("\r\n");
    LOG_DEBUG("calculate md5sum done.");

    f_close(&file);

    //unmount
    f_mount(0, "", 1);

    return 0;
}

uint8_t sd_card_sppend_file_data(uint8_t *file_data, uint16_t len, uint8_t last_pkt_flag)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t bw;
    uint8_t md5sum[32] = {0};

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return result;
    }

    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
        //unmount
        f_mount(0, "", 1);
        return result;
    }

    /*recreate tmp file for the first file packet, otherwise, append file */
    result = f_open(&file, g_tmp_fpga_file, FA_OPEN_APPEND | FA_WRITE);

    if (result != FR_OK) {
        LOG_ERROR("Don't Find File: %s.", g_tmp_fpga_file);
        //unmount
        f_mount(0, "", 1);
        return result;
    }
    result = f_write(&file, (void*)file_data, len, (UINT*)&bw);
    if (result != FR_OK) {
        LOG_ERROR("File Write Failed Error(%d)", result);
    }

    f_close(&file);

    //unmount
    f_mount(0, "", 1);

    if (0 == last_pkt_flag) {
        sd_card_calc_file_md5sum((char *)g_tmp_fpga_file, md5sum);
    }

    return result;
}

uint8_t sd_card_integrity_check(uint8_t type, uint8_t *recv_md5sum)
{
    uint8_t calc_md5sum[33] = { 0 };

    LOG_DEBUG("Check integrity of SD card tmp file:%s for upgrade:%d", g_tmp_fpga_file, type);
    sd_card_calc_file_md5sum(g_tmp_fpga_file, calc_md5sum);

    if (memcmp(calc_md5sum, recv_md5sum, 32)) {
        LOG_ERROR("Md5sum received(?) != calculated(%s), file transmission failed.", calc_md5sum);
        return MD5ERR;
    }

    if (FPGA_CE30 == type) {
        sd_card_rename_file(g_tmp_fpga_file, g_std_fsmc_fpga);
    }
    else {
        sd_card_rename_file(g_tmp_fpga_file, g_std_spi_fpga);
    }

    return DEVICEOK;
}

uint8_t sd_card_set_app_to_run(uint8_t type)
{
	(void)type;
	URESULT errcode;
	errcode = upgrade_ext_fpga((char*)g_std_fsmc_fpga_unzip);
	if(UP_OK == errcode)
		return SD_TRUE;
	return SD_FALSE;
}

//void sd_card_set_app_to_run(uint8_t type)
//{
//    FRESULT result;
//    FATFS fs;
//    FIL file;
//    DIR DirInf;
//    uint32_t bw;
//    int i;
//    char *f_name = NULL;
//
//    if (FSMC_FPGA == type) {
//        f_name = (char*)g_std_fsmc_fpga;
//        //FIXME: how to config FSMC FPGA
//        LOG_WARN("Try to config FSMC FPGA, run dummy operation");
//        return;
//    }
//    else {
//
//        f_name = (char*)g_std_spi_fpga;
//    }
//
//    /* 挂载文件系统 */
//    result = f_mount(&fs,"",1);         /* Mount a logical drive */
//    if (result != FR_OK) {
//        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
//    }
//
//    /* 打开根文件夹 */
//    result = f_opendir(&DirInf, "0:/"); /* 如果不带参数，则从当前目录开始 */
//    if (result != FR_OK)
//    {
//        LOG_ERROR("Root Directory is Open Error (%d)", result);
//        return;
//    }
//    else {
//        LOG_DEBUG("Root Directory is Open OK");
//    }
//
//    /* 打开文件 */
//    result = f_open(&file, f_name, FA_OPEN_EXISTING | FA_READ);
//    if (result !=  FR_OK)
//    {
//        LOG_ERROR("Don't Find File: %s", f_name);
//        return;
//    }
//    else {
//        LOG_DEBUG("Open File OK.");
//    }
//    /* 读取文件 */
//
//    for (i = 0;; i++) {
//        result = f_read(&file, &g_file_buff, sizeof(g_file_buff), (UINT*)&bw);
//        if (result || bw == 0) {
//            LOG_ERROR("Error: %d:%ld", result, bw);
//            break;
//        }
//        else {
//            // !i == 1, first flag is true, otherwise, false.
//            if (Config_FPGA(bw, (unsigned char*)g_file_buff, !i)) {
//                LOG_ERROR("Download PFGA FAIL %d", i);
//                break;
//            }
//        }
//    }
//
//    /* 关闭文件*/
//    f_close(&file);
//
//    /* 卸载文件系统 */
//    f_mount(0,"",1);
//
//    if (FSMC_FPGA == type) {
//        //FIXME: how to check FSMC FPGA work status
//    }
//    else {
//        LOG_INFO("Reconfig FPGA image result: %s.", !is_fpga_microcode_work_no_ok() ? "OK!" : "NOK!");
//    }
//}



/**
 * for SD card operator test
 * **/
//向SD中某个文件追加内容
void sd_card_append_file(char *filename, char *text)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t bw;

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return;
    }
    else {
        LOG_DEBUG("FileSystem Mounted OK");
    }

    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
    }
    else {
        LOG_DEBUG("Root Directory is Open OK");
    }

//    result = f_unlink(filename);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File: %s", filename);
        return;
    }
    else {
        LOG_DEBUG("Delete File OK.");
    }

    result = f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File: %s", filename);
        return;
    }
    else {
        LOG_DEBUG("Open File OK.");
    }

//    char *text = "FatFS Write Demo, hello world hi alphago, 01234567890!\r\n";
    result = f_write(&file, (void*)text, 256, (UINT*)&bw);//每次向文件中追加写入256Byte(一页）
    if (result == FR_OK) {
        LOG_DEBUG("%s File Write Success", filename);
    }
    else {
        LOG_ERROR("File Write Failed Error(%d)", result);
    }

    f_close(&file);

    //unmount
    f_mount(0, "", 1);
}

//计算SD卡中该文件的md5值
void sd_read_file(char *filename)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t i;
    uint32_t bw;
    md5_ctx_t ctx = {0};
    char md5sum[16] = {0};

    md5_init(&ctx);

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        LOG_ERROR("FileSystem Mounted Failed (%d)", result);
        return;
    }
    else {
        LOG_DEBUG("FileSystem Mounted OK");
    }

    result = f_opendir(&dir_info, "0:/");
    if (result != FR_OK) {
        LOG_ERROR("Root Directory is Open Error (%d)", result);
    }
    else {
        LOG_DEBUG("Root Directory is Open OK");
    }

    result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File: %s", filename);
        return;
    }
    else {
        LOG_DEBUG("Open File OK.");
    }

    for (i = 0; ; i++) {
        result = f_read(&file, &g_file_buff, sizeof(g_file_buff), (UINT*)&bw);
        if (result || bw == 0) {
//            printf("Read file Error (%d:%ld)\r\n", result, bw);
            break;
        }
        else {
//            LOG_INFO("read file len:%ld", bw);
            md5_update(&ctx, g_file_buff, bw);
            memset(g_file_buff, 0, sizeof(g_file_buff));
        }
    }
    md5_final(&ctx, md5sum);
    for (i = 0; i< 16; i++) {
        printf("%02x ", md5sum[i]);
    }
    LOG_DEBUG("read file and calculate md5sum done.");

    f_close(&file);

    //unmount
    f_mount(0, "", 1);
}

//测试向sd卡中某个文件追加信息
void sd_append_test(void)
{
    int i = 5;
    char *filename = "HELLO";
	uint8_t md5sum[32] = {0};
	char *text = "FatFS Write Demo, hello world 01234567890!\r\n";
    while (i--) {
        sd_card_append_file(filename, text);
    }
    //读取sd卡中该文件并打印输出，计算其md5
//    sd_read_file(filename);
//    filename = "fpga.rbf";

    //计算sd卡中某个文件的md5值
    sd_card_calc_file_md5sum(filename, md5sum);
}
