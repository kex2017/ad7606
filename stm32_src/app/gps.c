#include "periph/rtt.h"
#include "timex.h"
#include "xtimer.h"
#include "periph/uart.h"
#include "isrpipe.h"
#include "periph/gpio.h"

#define GPS_SW_PIN           GPIO_PIN(PORT_F,9)
#define GPS_SYNC_1HZ_IN_PIN   GPIO_PIN(PORT_D,3)
#define UART_GPS_DEV           UART_DEV(2)
#define UART_GPS_BAUDRATE      (9600)

#define GPS_RX_BUFSIZE    (512)
static char _rx_buf_mem[GPS_RX_BUFSIZE];
isrpipe_t uart_gps_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

void gps_init(void) {
   uart_init(UART_GPS_DEV, UART_GPS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_gps_isrpipe);
}

int gps_read(char* buffer, int count)
{
   (void) count;
   return isrpipe_read(&uart_gps_isrpipe, buffer, GPS_RX_BUFSIZE);
}

void gps_enable(void)
{
   tsrb_init(&(uart_gps_isrpipe.tsrb), _rx_buf_mem, GPS_RX_BUFSIZE);
}
