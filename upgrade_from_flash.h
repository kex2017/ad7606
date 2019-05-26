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


void flash_upload_file_handler_init(uint8_t type);

uint8_t flash_integrity_check(uint8_t file_type, uint8_t *recv_md5);

uint8_t flash_buf_append_file_data(uint8_t *file_data, uint16_t len, uint8_t last_pkt_flag);

uint8_t flash_set_app_to_run(uint8_t type);

void soft_reset(void);

#endif /* PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_FLASH_H_ */
