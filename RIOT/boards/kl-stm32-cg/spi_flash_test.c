/*
 * spi_flash_test.c
 *
 *  Created on: Nov 20, 2017
 *      Author: chenzy
 */

#include "../kl-stm32-cg/include/board.h"
#include "periph/spi.h"

#include "mtd_spi_nor.h"
#include "mtd.h"

#include "xtimer.h"
#include "timex.h"


#define SLEEP       (100 * US_PER_MS)


void spi_flash_id_test(void)
{
   uint8_t fac_id, dev_id;
   int tmp;
   tmp = spi_init_cs(SPI_DEV(0), spi_config[0].cs_pin);
   if (tmp != SPI_OK) {
       puts("error: unable to initialize the given chip select line");
       return ;
   }

   int mode = SPI_MODE_3;
   int clk = SPI_CLK_10MHZ;
   tmp = spi_acquire(SPI_DEV(0), spi_config[0].cs_pin, mode, clk);
   if (tmp == SPI_NOMODE) {
       puts("error: given SPI mode is not supported");
       return ;
   }
   else if (tmp == SPI_NOCLK) {
       puts("error: targeted clock speed is not supported");
       return ;
   }
   else if (tmp != SPI_OK) {
       puts("error: unable to acquire bus with given parameters");
       return ;
   }

   printf("SPI_DEV(0) initialized: mode: %i, clk: %i\n", mode, clk);

   uint8_t outb[] = {0x90, 0x00, 0x00, 0x00};
   spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &outb, NULL, 4);
   spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, NULL, &fac_id, 1);
   spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, NULL, &dev_id, 1);

   spi_release(SPI_DEV(0));

   printf("Flash id %d %d\r\n", fac_id, dev_id);
}


void wip(void)
{
    uint8_t v = 1;
    uint8_t rdsr_ops = 0x05;

    while(1 == (v & 0x01)) {
        spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &rdsr_ops, NULL, 1);
        spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, NULL, &v, 1);
//        printf("wip v:%d, 1&v:%d.\r\n", v, v & 0x01);
    }
}

#define MAX_FPGA_DATA_LEN (0)//0x6000
//#define MAX_FPGA_DATA_LEN (20480 + 4096)//0x6000
void spi_flash_erase_test(void)
{
    int mode = SPI_MODE_3;
    int clk = SPI_CLK_10MHZ;
    int tmp;

    tmp = spi_acquire(SPI_DEV(0), spi_config[0].cs_pin, mode, clk);
    if (tmp == SPI_NOMODE) {
        puts("error: given SPI mode is not supported");
        return;
    }
    else if (tmp == SPI_NOCLK) {
        puts("error: targeted clock speed is not supported");
        return;
    }
    else if (tmp != SPI_OK) {
        puts("error: unable to acquire bus with given parameters");
        return;
    }

    printf("SPI_DEV(0) initialized: mode: %i, clk: %i\r\n", mode, clk);

    //wsr
//    uint8_t wsr_ops = 0x50;
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &wsr_ops, NULL, 1);
//
//    uint8_t outb[] = {0x01, 0x00};
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &outb, NULL, 2);
//    puts("wsr_ops ok.\r\n");
//
//    wip();
//    puts("wip ok.\r\n");

    //wen
    uint8_t wen_ops = 0x06;
    puts("wen ready.\r\n");
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &wen_ops, NULL, 1);
    puts("wen ok.\r\n");

    //erase
    uint8_t erase_ops = 0x20;
    uint8_t erase_addr[] = {(MAX_FPGA_DATA_LEN&0xffffff)>>16, (MAX_FPGA_DATA_LEN&0xffff)>>8, (MAX_FPGA_DATA_LEN&0xff)}; //erase section 0
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &erase_ops, NULL, 1);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &erase_addr, NULL, 3);
    puts("erase ok\r\n");

    //wip==rdsr
    wip();
    puts("wip ok.\r\n");

    spi_release(SPI_DEV(0));
}

void spi_flash_read_test(void)
{
   int tmp;
   int i;
	int mode = SPI_MODE_3;
	int clk = SPI_CLK_10MHZ;
	tmp = spi_acquire(SPI_DEV(0), spi_config[0].cs_pin, mode, clk);
    if (tmp == SPI_NOMODE) {
        puts("error: given SPI mode is not supported");
        return;
    }
    else if (tmp == SPI_NOCLK) {
        puts("error: targeted clock speed is not supported");
        return;
    }
    else if (tmp != SPI_OK) {
        puts("error: unable to acquire bus with given parameters");
        return;
    }

   printf("SPI_DEV(0) initialized: mode: %i, clk: %i\r\n", mode, clk);

	uint8_t read_ops = 0x03;
	uint8_t read_addr[] = {(MAX_FPGA_DATA_LEN&0xffffff)>>16, (MAX_FPGA_DATA_LEN&0xffff)>>8, (MAX_FPGA_DATA_LEN&0xff)}; //read addr
	uint8_t read_buf[100] = {0};
	for (i = 0; i < 100; i++) {
	    read_buf[i] = 10;
	}

    printf("Before read data: %d, %d..\r\n", read_buf[0], read_buf[99]);

    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &read_ops, NULL, 1);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &read_addr, NULL, 3);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, NULL, &read_buf, 7);
    printf("After read data: %d, %d, %d, %d, %d, %d, %d..\r\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5], read_buf[6]);

//    wip();

	spi_release(SPI_DEV(0));
}

void spi_flash_byte_write_test(void)
{
    int tmp;
    int mode = SPI_MODE_3;
    int clk = SPI_CLK_10MHZ;
    tmp = spi_acquire(SPI_DEV(0), spi_config[0].cs_pin, mode, clk);
    if (tmp == SPI_NOMODE) {
        puts("error: given SPI mode is not supported");
        return;
    }
    else if (tmp == SPI_NOCLK) {
        puts("error: targeted clock speed is not supported");
        return;
    }
    else if (tmp != SPI_OK) {
        puts("error: unable to acquire bus with given parameters");
        return;
    }

    printf("SPI_DEV(0) initialized: mode: %i, clk: %i\r\n", mode, clk);

//    //wsr
//    uint8_t wsr_ops = 0x50;
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &wsr_ops, NULL, 1);
//
//    uint8_t outb[] = { 0x01, 0x00 };
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &outb, NULL, 2);
//    puts("wsr_ops ok.\r\n");
//
//    wip();
//    puts("wip ok.\r\n");

    //wen
    uint8_t wen_ops = 0x06;
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &wen_ops, NULL, 1);
    puts("wen ok.\r\n");

    uint8_t write_ops = 0x02;
    uint8_t write_addr[] = {(MAX_FPGA_DATA_LEN&0xffffff)>>16, (MAX_FPGA_DATA_LEN&0xffff)>>8, (MAX_FPGA_DATA_LEN&0xff)}; //write section 0
    uint8_t write_data0 = 9;
    uint8_t write_data1 = 10;

    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &write_ops, NULL, 1);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &write_addr, NULL, 3);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &write_data0, NULL, 1);
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &write_data1, NULL, 1);
    printf("write data: %d %d..\r\n", write_data0, write_data1);

    wip();
    spi_release(SPI_DEV(0));
}

void spi_flash_aai_word_write_test(void)
{
    int tmp;
    int mode = SPI_MODE_3;
    int clk = SPI_CLK_10MHZ;
    tmp = spi_acquire(SPI_DEV(0), spi_config[0].cs_pin, mode, clk);
    if (tmp == SPI_NOMODE) {
        puts("error: given SPI mode is not supported");
        return;
    }
    else if (tmp == SPI_NOCLK) {
        puts("error: targeted clock speed is not supported");
        return;
    }
    else if (tmp != SPI_OK) {
        puts("error: unable to acquire bus with given parameters");
        return;
    }

    printf("SPI_DEV(0) initialized: mode: %i, clk: %i\r\n", mode, clk);

    //ebsy
//    uint8_t ebsy_ops = 0x70;
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &ebsy_ops, NULL, 1);

    //wsr
//    uint8_t wsr_ops = 0x50;
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &wsr_ops, NULL, 1);
//
//    uint8_t outb[] = { 0x01, 0x00 };//0x00 status register
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &outb, NULL, 2);
//    puts("wsr_ops ok.\r\n");
//
//    wip();
//    puts("wip ok.\r\n");

    //wen
    uint8_t wen_ops = 0x06;
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &wen_ops, NULL, 1);
    puts("wen ok.\r\n");

    uint8_t aai_ops = 0xAD;
    uint8_t write_addr[] = {(MAX_FPGA_DATA_LEN&0xffffff)>>16, (MAX_FPGA_DATA_LEN&0xffff)>>8, (MAX_FPGA_DATA_LEN&0xff)}; //write section 0
    uint8_t write_data[] = {1, 4};

//    gpio_clear(spi_config[0].cs_pin);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &aai_ops, NULL, 1);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &write_addr, NULL, 3);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &write_data, NULL, 2);
    wip();
    printf("write data: %d, %d..\r\n", write_data[0], write_data[1]);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &aai_ops, NULL, 1);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &write_data, NULL, 2);
    wip();
    printf("write data: %d, %d..\r\n", write_data[0], write_data[1]);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 1, &aai_ops, NULL, 1);
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &write_data, NULL, 2);
    printf("write data: %d, %d..\r\n", write_data[0], write_data[1]);
//    wip(); //bug here

    //wrdi
    uint8_t write_dis_ops = 0x04;
    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &write_dis_ops, NULL, 1);
    wip();

//    dbsy
//    uint8_t dbsy_ops = 0x80;
//    spi_transfer_bytes(SPI_DEV(0), spi_config[0].cs_pin, 0, &dbsy_ops, NULL, 1);

//    wip();

    spi_release(SPI_DEV(0));
}

void delay_mm(void)
{
    int i = 0, j = 0;

    for (j = 0; j < 1000; j++) {
        for (i = 0; i < 1000; i++) {
            i++;
        }
    }

}

void spi_flash_test(void)
{
	spi_flash_id_test();
	puts("Start erase test.\r\n");
	spi_flash_erase_test();
	spi_flash_read_test();
	spi_flash_read_test();
	puts("Erase test done\r\n");

//	puts("Start byte write test.\r\n");
//	spi_flash_byte_write_test();
//	spi_flash_read_test();
//	puts("Byte write test done\r\n");

	puts("Start AAI word write test.\r\n");
	spi_flash_aai_word_write_test();
   spi_flash_read_test();
   puts("AAI word write test done\r\n");

}

static mtd_spi_nor_t klstm32_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .sector_count = 32,
        .pages_per_sector = 16,
        .page_size = 4096,
    },
    .opcode = &mtd_spi_nor_opcode_default,
    .spi = KLSTM32_NOR_SPI_DEV,
    .cs = KLSTM32_NOR_SPI_CS,
    .addr_width = 3,
    .mode = SPI_MODE_3,
    .clk = SPI_CLK_10MHZ,
    .flag = SPI_NOR_F_SECT_4K|SPI_NOR_F_SECT_32K

};

mtd_dev_t *mtd0 = (mtd_dev_t *)&klstm32_nor_dev;

int mtd_flash_init(mtd_dev_t *mtd)
{
   int res = mtd_init(mtd);
   if (res != 0) {
       printf("error: MTD SPI Flash init Error: %d", res);
   }
   puts("MTD SPI Flash init OK.\r\n");
   return res;
}

void mtd_flash_byte_write_test_ok(void)
{
    uint8_t data_buf[6] = { 0 };
    mtd_read(mtd0, &data_buf, 0, 6);
    printf("Mtd before byte write erase as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
           data_buf[4], data_buf[5]);

    mtd_erase(mtd0, 0, 4096);
    mtd_read(mtd0, &data_buf, 0, 6);
    printf("Mtd after byte write erase data as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
           data_buf[4], data_buf[5]);

    uint8_t i = 6;
    for (i = 0; i < 6; i++) {
        data_buf[i] = 4;
    }
    mtd_write(mtd0, &data_buf, 0, 1);
    printf("Mtd before byte write data as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
               data_buf[4], data_buf[5]);
    mtd_read(mtd0, &data_buf, 0, 6);
    printf("Mtd after byte write data as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
           data_buf[4], data_buf[5]);
}

void mtd_flash_aai_write_test(void)
{
    uint8_t data_buf[6] = { 0 };
    uint8_t buf[6] = { 0 };
    mtd_read(mtd0, &data_buf, 0, 6);
    printf("Mtd before aai erase as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
           data_buf[4], data_buf[5]);

    mtd_erase(mtd0, 0, 4096);
    mtd_read(mtd0, &data_buf, 0, 6);
    printf("Mtd after aai erase data as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
           data_buf[4], data_buf[5]);

    uint8_t i = 6;
    for (i = 0; i < 6; i++) {
        data_buf[i] = 7;
    }
    mtd_write(mtd0, &data_buf, 0, 6);
    printf("Mtd before aai write data as: %d, %d, %d, %d, %d, %d.\r\n", data_buf[0], data_buf[1], data_buf[2], data_buf[3],
               data_buf[4], data_buf[5]);
    mtd_read(mtd0, &buf, 0, 6);
    printf("Mtd after aai write data as: %d, %d, %d, %d, %d, %d.\r\n", buf[0], buf[1], buf[2], buf[3],
           buf[4], buf[5]);
}

void mtd_flash_test(void)
{
    mtd_flash_init(mtd0);
    mtd_flash_byte_write_test_ok();
    mtd_flash_aai_write_test();
}

