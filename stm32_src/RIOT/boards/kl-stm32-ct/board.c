/**
 * @ingroup     boards_kelin_stm32f103ze
 * @{
 *
 * @file
 * @brief       Board specific implementations for the kelin-stm32 board
 *
 * @author      Xu Bingcan <xubingcan@klec.com.cn>
 *
 * @}
 */

#include "../kl-stm32-ct/include/board.h"

#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "cpu_conf.h"
#include "stdio_uart.h"
#include "periph/gpio.h"
#include "periph/rtt.h"


#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field.
                                  This value must be a multiple of 0x200. */




void board_init(void)
{
   cpu_init();
   rtt_init();
   stdio_init();
}


