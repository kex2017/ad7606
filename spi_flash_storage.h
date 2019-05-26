/*
 * flash_storage.h
 *
 *  Created on: Dec 28, 2017
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_FLASH_STORAGE_H_
#define PARTS_CABLE_GROUND_NEW_FLASH_STORAGE_H_

#include <stdint.h>


uint32_t flash_init(void);
uint32_t flash_read_data(void *dest, uint32_t addr, uint32_t size);

/**
 * @brief write data to flash
 * the min operate data size is page size(4096), if size<4096,
 * data in the page must be hold before write, or the data will be lost
 *
 * @param[in] src       source data pointer
 * @param[in] addr      flash address to write
 * @param[in] size      length of data to write
 *
 * @return              0 on success, else failure
 **/
uint32_t flash_write_data(const void *src, uint32_t addr, uint32_t size);


#endif /* PARTS_CABLE_GROUND_NEW_FLASH_STORAGE_H_ */
