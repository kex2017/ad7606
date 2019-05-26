/*
 * flash_storage.c
 *
 *  Created on: Dec 28, 2017
 *      Author: chenzy
 */

#include "log.h"
#include "spi_flash_storage.h"

#include "periph/spi.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "board.h"

#define PAGE_SIZE (4096)


static mtd_spi_nor_t klstm32_nor_spi_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .sector_count = 512,
        .pages_per_sector = 1,
        .page_size = PAGE_SIZE,
    },
    .opcode = &mtd_spi_nor_opcode_default,
    .spi = KLSTM32_NOR_SPI_DEV,
    .cs = KLSTM32_NOR_SPI_CS,
    .addr_width = 3,
    .mode = SPI_MODE_3,
    .clk = SPI_CLK_10MHZ,
    .flag = SPI_NOR_F_SECT_4K|SPI_NOR_F_SECT_32K

};

mtd_dev_t *spi_mtd0 = (mtd_dev_t *)&klstm32_nor_spi_dev;


uint32_t flash_init(void)
{
   int res = mtd_init(spi_mtd0);
   if (res != 0) {
       LOG_ERROR("error: MTD SPI Flash init Error: %d", res);
       return res;
   }
   LOG_INFO("MTD SPI Flash init OK.\r\n");
   return res;
}

uint32_t flash_read_data(void *dest, uint32_t addr, uint32_t size)
{
    uint32_t ret = 0;
    uint16_t i = 0;
    uint32_t read_size = size;

    for (i = 0; i <= size / PAGE_SIZE; i++) {
        ret = mtd_read(spi_mtd0, dest + i * PAGE_SIZE, addr + i * PAGE_SIZE,
                       read_size > PAGE_SIZE ? PAGE_SIZE : read_size);
        read_size -= PAGE_SIZE;
    }

    return ret;
}


uint32_t flash_write_data(const void *src, uint32_t addr, uint32_t size)
{
    uint32_t ret = 0;
    uint16_t i = 0;
    uint32_t write_size = size;

    if (size < PAGE_SIZE) {
        LOG_WARN("Write flash size < PAGE_SIZE, the rest data of page will lost.");
    }

    for (i = 0; i <= size / PAGE_SIZE; i++) {
        mtd_erase(spi_mtd0, addr + i * PAGE_SIZE, PAGE_SIZE);
        ret = mtd_write(spi_mtd0, src + i * PAGE_SIZE, addr + i * PAGE_SIZE,
                        write_size > PAGE_SIZE ? PAGE_SIZE : write_size);
        write_size -= PAGE_SIZE;
    }

    return ret;
}
