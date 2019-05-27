#include "../kl-cg-ibox-new/include/board.h"

#include "../kl-cg-ibox-new/include/periph_conf.h"
#include "cpu.h"
#include "cpu_conf.h"

#include "periph/rtt.h"
#include "stdio_uart.h"
#include "periph/gpio.h"


void board_init(void)
{
   /* initialize the CPU */
   cpu_init();
//   rtt_poweron();
   rtt_init();
   stdio_init();
}
