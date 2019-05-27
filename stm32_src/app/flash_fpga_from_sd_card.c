/*
 * upgrade_from_sd_card.c
 *
 *  Created on: Jan 3, 2018
 *      Author: chenzy
 */

#include "flash_fpga_from_sd_card.h"

#include <stdio.h>
#include "type_alias.h"
#include "dev_cfg.h"

#include "log.h"
#include "fatfs/ff.h"
#include "hashes/md5.h"

#include "klfpga.h"
#include "mtd.h"

const char *g_tmp_fpga_file = "tmp_fpga.rbf";
const char *g_std_fsmc_fpga = "fpga_fsmc.rbf";
const char *g_std_spi_fpga = "IGCB.bin";

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


uint8_t clean_file_directory(const char *filename)
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
    clean_file_directory(g_tmp_fpga_file);
}

uint8_t sd_card_calc_file_md5sum(const char *filename, uint8_t *md5sum)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t bw;
    uint32_t i;
    char buf[2000] = {0};
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
        result = f_read(&file, &buf, sizeof(buf), (UINT*)&bw);
        if (result || bw == 0) {
//            printf("Read file Error (%d:%ld)\r\n", result, bw);
            break;
        }
        else {
            LOG_DEBUG("read file len:%ld", bw);
            md5_update(&ctx, buf, bw);
            memset(buf, 0, sizeof(buf));
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

uint8_t sd_card_sppend_file_data(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t bw;
    uint8_t md5sum[32] = {0};
    (void)file_index;

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

    if (FSMC_FPGA == type) {
        sd_card_rename_file(g_tmp_fpga_file, g_std_fsmc_fpga);
    }
    else {
        sd_card_rename_file(g_tmp_fpga_file, g_std_spi_fpga);
    }

    return DEVICEOK;
}

void sd_card_set_app_to_run(void)
{
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;
	uint32_t bw;
	unsigned char buf[512];
	int i;
	char *f_name = NULL;

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

	LOG_INFO("Reconfig FPGA image result: %s.", !is_fpga_microcode_work_no_ok() ? "OK!" : "NOK!");
}



/**
 * for SD card operator test
 * **/
void sd_card_append_file(char *filename)
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

    char *text = "FatFS Write Demo, hello world, 01234567890!\r\n";
    result = f_write(&file, (void*)text, strlen(text), (UINT*)&bw);
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

void sd_read_file(char *filename)
{
    FATFS fs;
    FRESULT result;
    DIR dir_info;
    FIL file;
    uint32_t i;
    uint32_t bw;
    char buf[2000] = {0};
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
        result = f_read(&file, &buf, sizeof(buf), (UINT*)&bw);
        if (result || bw == 0) {
//            printf("Read file Error (%d:%ld)\r\n", result, bw);
            break;
        }
        else {
            LOG_INFO("read file len:%ld", bw);
            md5_update(&ctx, buf, bw);
            memset(buf, 0, 2000);
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
void sd_append_test(void)
{
    int i = 5;
    char *filename = "TEST8.TXT";

    uint8_t md5sum[32] = {0};

    while (i--) {
        sd_card_append_file(filename);
    }

    sd_read_file(filename);
    filename = "fpga.rbf";

    sd_card_calc_file_md5sum(filename, md5sum);
}
