/*
 * kl_fpga_update.h
 *
 *  Created on: Jun 25, 2018
 *      Author: chenzy
 */
#include <stdint.h>
#include "kl_soft_spi.h"

#ifndef EXT_FPGA_IMG_UPGRADE_H_
#define EXT_FPGA_IMG_UPGRADE_H_

#define FPGA_FLASH_SIZE (8 * 1024 *1024)

typedef enum {
	UP_OK = 0,
	UP_FS_MOUNT_ERR,
	UP_FS_OPEN_DIR_ERR,
	UP_UNCOMPRESS_ERR,
	UP_LOAD_FPGA_IMG_ERR,
	UPGRADE_ERR,
}URESULT;

void read_kl_fpga_flash_id(void);
void read_fpga_flash_bytes(uint32_t addr, uint32_t len);
void write_fpga_flash_bytes(uint32_t addr, uint8_t* buff, int len);
void erase_fpga_flash(void);

int uncompress_file(char *compressed_file, const char *uncompressed_file);

uint8_t calc_ext_fpga_img_md5sum(const char *filename, uint8_t *md5sum);
void calc_ext_fpga_flash_md5sum(int32_t total_len, uint8_t *md5sum);

int download_ext_fpga_img(const char *input_file);
void bak_kl_fpga_img(int32_t total_len, const char *input_file);


int upgrade_ext_fpga(char *compressed_file);

#endif /* EXT_FPGA_UPDATE_H_ */
