/*
 * upgrade_from_flash.h
 *
 *  Created on: Jan 5, 2018
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_
#define PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_

#include <stdint.h>


#define FLASH_PAGE_SIZE    ((uint16_t)0x800)

void mock_upgrade_file_len(uint32_t len);

void soft_reset(void);

void flash_upload_file_handler_init(uint8_t type);

uint8_t flash_integrity_check(uint8_t file_type, uint8_t *recv_md5);

uint8_t hardcode_addr_flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag);
uint8_t flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag);

void flash_set_app_to_run(uint8_t type);

uint32_t get_device_cfg_flash_page_addr(void);



#endif /* PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_ */
