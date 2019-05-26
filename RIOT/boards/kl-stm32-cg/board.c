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

#include "../kl-stm32-cg/include/board.h"

#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>

#include "uart_stdio.h"
#include "cpu.h"
#include "cpu_conf.h"

#include "periph/gpio.h"
#include "periph/fsmc.h"
#include "periph/rtt.h"


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

void fpga_read_write_test(void)
{
   char str[] = "HelloWorld!";
   char buf[32];
   memset(buf, 0, sizeof(buf));
   memcpy((void*)FPGA_BASE_ADDR, str, sizeof(str));
   memcpy((void*)buf, (void*)FPGA_BASE_ADDR, sizeof(str));
   puts("fpga ram read/write OK.");
}

void IWDG_init(void)
{
    IWDG->KR = 0X5555;    //解除寄存器保护
    IWDG->PR = 7;         // 写入初始化分频值
    IWDG->RLR = 0x0fff;   // 写入自动装载值
    IWDG->KR = 0XAAAA;    //开启寄存器保护
    IWDG->KR = 0XCCCC;    //启动看门狗

    puts("IWDG_init OK.");
}

void IWDG_Feed(void)
{
    IWDG->KR=0XAAAA;
}

void board_init(void)
{
   /* initialize the CPU */

   cpu_init();
   fsmc_init();
   rtt_init();

   /*Init UART as stdio...see include/periph_conf.h */
   uart_stdio_init();
//   sram_read_write_test();
//   fpga_read_write_test();

//   spi_flash_test();
//   mtd_flash_test();

   /* NVIC PRIGROUP
    * 3	priority:[7:4], subprio [3:0]
    * 4	priority:[7:5], subprio [4:0]
    * 5	priority:[7:6], subprio [5:0]
    * 6	priority:[7:7], subprio [6:0]
    * only high 4-bit used in STM32,
    * so 16 priority with 0 subprio here
    * */
   NVIC_SetPriorityGrouping(3);
}

void watch_dog_init(void)
{
   gpio_init(DOG_PIN, GPIO_OUT);
}

