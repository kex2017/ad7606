/*
 * upgrade_from_flash.h
 *
 *  Created on: Jan 5, 2018
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_
#define PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_

#include <stdint.h>


#define CFG_OK 1
#define CFG_NOK 0

#define FLASH_PAGE_SIZE    ((uint16_t)0x800)

#define ENV_DEVICE_START_BASE_ADDR    0x08078800  //34k
#define ENV_START_BASE_ADDR           0x08078000  //32k
#define PROGRAM_FPGA_BASE_ADDR        0x08050000  //160k
#define PROGRAM_UPGRADE_BASE_ADDR     0x08030000  //128k
#define PROGRAM_ACTION_BASE_ADDR      0x08010000  //128k
#define CPU_FLASH_BASE_ADDR           0x08000000  //512k
#define FLAG_CLEAR    0x12121200
#define FLAG_SET      0x21212100

void mock_upgrade_file_len(uint32_t len);

void soft_reset(void);

void flash_upload_file_handler_init(uint8_t type);

uint8_t flash_integrity_check(uint8_t file_type, uint8_t *recv_md5);

uint8_t flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint8_t last_pkt_flag);

void flash_set_app_to_run(uint8_t type);

uint32_t get_device_cfg_flash_page_addr(void);

uint32_t get_fpga_start_flash_page_addr(void);

uint32_t get_fpga_start_flash_addr(void);

void download_fpga_image(uint8_t fpga_no);

uint8_t check_fpga_cfg_status(uint8_t fpga_no);

#endif /* PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_ */
