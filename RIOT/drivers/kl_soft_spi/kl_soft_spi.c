/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_soft_spi
 * @{
 *
 * @file
 * @brief       Software SPI implementation
 *
 * @author      Markus Blechschmidt <Markus.Blechschmidt@haw-hamburg.de>
 * @author      Peter Kietzmann     <peter.kietzmann@haw-hamburg.de>
 */
#include <stdio.h>
#include <assert.h>

#include "mutex.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "kl_soft_spi.h"
#include "soft_spi.h"
#include "soft_spi_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define     FPGA_BASE_ADDR            0x60000000
static void  write_fpga_reg(uint32_t offset, uint16_t data)
{
   *((volatile unsigned short*)(FPGA_BASE_ADDR + (uint32_t)((uint32_t)offset << 17))) = (unsigned short)(data);
}
static uint16_t  read_fpga_reg(uint32_t offset)
{
   return *((volatile unsigned short*)(FPGA_BASE_ADDR + (uint32_t)((uint32_t)offset << 17)));
}

/**************************************************
 * mock SOFT SPI bit operations for EXT FPGA
**************************************************/

const uint16_t SPI_REG = 9;
const uint16_t SPI_COPY_REG = 7;
const uint16_t SPI_MISO_REG = 6;

//cs 1; clk 1; mosi:0
static uint16_t g_last_reg_value = 0x0006;
static void write_ext_fpga_soft_spi_bit(KL_SPI_PIN pin, int8_t bit_value)
{
    uint16_t w_value;

//    r_value = read_reg(SPI_REG);
    if (bit_value) {
        w_value = g_last_reg_value | ((1 << pin) & 0xffff);
    }
    else {
        w_value = g_last_reg_value & (~((1 << pin) & 0xffff));
    }
//    printf("reg_value:0x%04x\r\n", w_value);
//    写的时候，麻烦把写9寄存器的值往寄存器7同样写一份。例如要往寄存器9写5，fpga_write(9,5);
//    紧跟着往寄存器7也写一个同样的数据，fpga_write(7,5);
    write_fpga_reg(SPI_REG, w_value);
    write_fpga_reg(SPI_COPY_REG, w_value);
    g_last_reg_value = w_value;
}

static int8_t read_ext_fpga_soft_spi_bit(KL_SPI_PIN pin)
{
    uint16_t value;

    if (pin == KL_SPI_MISO) {
        value = read_fpga_reg(SPI_MISO_REG);
    }
    else {
    	value = g_last_reg_value;
        //value = read_reg(SPI_REG);
    }
//  printf("value:0x%04x, bit:%d\r\n", value, (value >> pin) & 0x0001);

    return (int8_t)(value >> pin) & 0x0001;
}

void toggle_ext_fpga_soft_spi_bit(KL_SPI_PIN pin)
{
    int8_t value;

    value = read_ext_fpga_soft_spi_bit(pin);
    if (value) {
        write_ext_fpga_soft_spi_bit(pin, 0);
    }
    else {
        write_ext_fpga_soft_spi_bit(pin, 1);
    }
}

/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[sizeof soft_spi_config];

static inline bool soft_spi_bus_is_valid(soft_spi_t bus)
{
    unsigned int soft_spi_num = (unsigned int) bus;

    if (sizeof(soft_spi_config) / sizeof(soft_spi_config[0]) < soft_spi_num) {
        return false;
    }
    return true;
}

void kl_soft_spi_init(soft_spi_t bus)
{
    DEBUG("Soft SPI init\n");

    assert(soft_spi_bus_is_valid(bus));

    /* initialize device lock */
    mutex_init(&locks[bus]);
//    kl_soft_spi_init_pins(bus);
}

//soft_spi_mode always SOFT_SPI_MODE_3
//soft_spi_clk always 100KHZ(5000ns)
//static int8_t g_last_clk_value = 1;
int kl_soft_spi_acquire(soft_spi_t bus, KL_SPI_PIN cs, soft_spi_mode_t mode, soft_spi_clk_t clk)
{
    (void)cs;
    (void)mode;
    (void)clk;

	/* lock bus */
	mutex_lock(&locks[bus]);
	DEBUG("lock bus");
	write_ext_fpga_soft_spi_bit(KL_SPI_CLK, 1);//初始化时钟引脚为高电平

    return 0;
}

void kl_soft_spi_release(soft_spi_t bus)
{
	mutex_unlock(&locks[bus]);
	DEBUG("unlock bus");
}

static inline uint8_t _transfer_one_byte(soft_spi_t bus, uint8_t out)
{
    int8_t bit = 0, i = 0;
    toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);

    bit = (out & (1 << 7)) >> 7;
    write_ext_fpga_soft_spi_bit(KL_SPI_MOSI, bit);
    for (i = 6; i >= 0; i--) {
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);//1
        toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);//2
        toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);
        bit = (out & (1 << i)) >> i;
        write_ext_fpga_soft_spi_bit(KL_SPI_MOSI, bit);
    }
    xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
    toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);

    return out;
}

static inline uint8_t _readin_one_byte(soft_spi_t bus, uint8_t* in)
{
    int8_t  i = 0, tmp = 0;
    int bit;
//    if (SOFT_SPI_MODE_1 == soft_spi_config[bus].soft_spi_mode ||
//        SOFT_SPI_MODE_3 == soft_spi_config[bus].soft_spi_mode) {
//        /* CPHA = 1*/
//        gpio_toggle(soft_spi_config[bus].clk_pin);
//    }
    toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);

    bit = read_ext_fpga_soft_spi_bit(KL_SPI_MISO);
    tmp |= (!!bit << 7);
    for (i = 6; i >= 0; i--) {
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);
        bit = read_ext_fpga_soft_spi_bit(KL_SPI_MISO);
        tmp |= (!!bit << i);
    }
    *in= tmp;
    xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
    toggle_ext_fpga_soft_spi_bit(KL_SPI_CLK);

    return *in;
}

uint8_t kl_soft_spi_transfer_byte(soft_spi_t bus, KL_SPI_PIN cs, bool cont, uint8_t out)
{
    DEBUG("Soft SPI soft_spi_transfer_byte\n");
    assert(soft_spi_bus_is_valid(bus));

    uint8_t retval = 0;

    write_ext_fpga_soft_spi_bit(cs, 0);//拉低片选
    retval = _transfer_one_byte(bus, out);

    if (!cont) {
    	write_ext_fpga_soft_spi_bit(cs, 1);//拉高片选
    }

    return retval;
}

uint8_t kl_soft_spi_readin_byte(soft_spi_t bus, KL_SPI_PIN cs, bool cont, uint8_t* in)
{
    DEBUG("Soft SPI soft_spi_readin_byte\n");
    assert(soft_spi_bus_is_valid(bus));

    uint8_t retval = 0;

    write_ext_fpga_soft_spi_bit(cs, 0);//拉低片选
    retval = _readin_one_byte(bus, in);
    if (!cont) {
        	write_ext_fpga_soft_spi_bit(cs, 1);//拉高片选
        }

    return retval;
}

void kl_soft_spi_transfer_bytes(soft_spi_t bus, KL_SPI_PIN cs, bool cont,
                        const void *out, void *in, size_t len)
{
    DEBUG("Soft SPI soft_spi_transfer_bytes\n");

    assert(soft_spi_bus_is_valid(bus));

    uint8_t tmp = 0;

    if (NULL != out) {
        for (size_t i = 0; i < len - 1; i++) {
            tmp = (NULL != out) ? ((uint8_t *)out)[i] : 0;
            uint8_t retval = kl_soft_spi_transfer_byte(bus, cs, true, tmp);
            if (NULL != in) {
                ((uint8_t *)in)[0] = retval;
            }
        }

        tmp = (NULL != out) ? ((uint8_t *)out)[len - 1] : 0;
        kl_soft_spi_transfer_byte(bus, cs, cont, tmp);
    }

    if (NULL != in) {
        for (size_t i = 0; i < len - 1; i++) {
            uint8_t retval = kl_soft_spi_readin_byte(bus, cs, true, &tmp);
            if (NULL != in) {
                ((uint8_t *)in)[i] = retval;
            }
        }

        kl_soft_spi_readin_byte(bus, cs, cont, &tmp);
        if (NULL != in) {
            ((uint8_t *)in)[len - 1] = tmp;
        }
    }
}
