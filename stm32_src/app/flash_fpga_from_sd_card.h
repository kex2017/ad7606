/*
 * upgrade_fpga.h
 *
 *  Created on: Jan 3, 2018
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_SD_CARD_H_
#define PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_SD_CARD_H_

#include <stdint.h>

void sd_append_test(void);


/*
 * @brief received first file transmission packet, do something for prepare
 *
 * @param[in] type         FPGA type: FSMC or SPI
 * */
void sd_card_upload_file_handler_init(uint8_t type);


/**
 * @brief append every file transmission packet to SD card file
 *
 * @param[in] file_data       file to write
 * @param[in] len             data length
 * @param[in] last_pkt_flag   when last_pkt_flag==0 means it's the last packet
 *
 * return                     0, success; else, failure and return error code
 * */
uint8_t sd_card_sppend_file_data(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag);


/**
 * @brief calculate md5sum from SD card file, and compare with received md5sum
 *
 * @param[in] type         FPGA type: FSMC or SPI
 * @param[in] recv_md5sum  received md5sum
 *
 * return                  0, success; else, failure and return error code
 */
uint8_t sd_card_integrity_check(uint8_t type, uint8_t *recv_md5sum);


/**
 * @brief config image to FPGA
 *
 * @param[in] type         FPGA type: FSMC or SPI
 *
 * return                  0, success; else, failure and return error code
 * */
void sd_card_set_app_to_run(void);



#endif /* PARTS_CABLE_GROUND_NEW_UPGRADE_FROM_SD_CARD_H_ */
