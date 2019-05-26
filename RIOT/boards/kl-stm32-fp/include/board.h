#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "../../kl-stm32-fp/include/periph_conf.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XTIMER_WIDTH        (16)
#define     FPGA_BASE_ADDR            0x60000000
#define     SRAM_BASE_ADDR            0x64000000
#define     DM9000_BASE_ADDR          0x6C000000


void show_hwclock(void);
void set_hwclock(uint8_t year, uint8_t mon, uint8_t day, uint8_t hours, uint8_t min, uint8_t sec);
void pcf8563_set_time(void);

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif
