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

#include "../kl-stm32-fp/include/board.h"

#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>

#include "uart_stdio.h"
#include "cpu.h"
#include "cpu_conf.h"


#include "periph/gpio.h"
#include "periph/rtt.h"
#include "periph/fsmc.h"


#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field.
                                  This value must be a multiple of 0x200. */

void sram_read_write_test(void)
{
   char str[] = "HelloWorld!";
   char buf[32];
   memset(buf, 0, sizeof(buf));
   memcpy((void*)SRAM_BASE_ADDR, str, sizeof(str));
   memcpy((void*)buf, (void*)SRAM_BASE_ADDR, sizeof(str));
   if (memcmp(buf, str, sizeof(str)) == 0) {
      puts("sram read/write OK.");
   } else {
      puts("sram read/write NOK.NOK.NOK");
   }
}

void board_init(void)
{
   /* initialize the CPU */
   cpu_init();
   rtt_init();
//   fsmc_init();
   uart_stdio_init();


//   sram_read_write_test();

   /*Init UART as stdio...see include/periph_conf.h */

}

