#include "../kl-stm32-cg/include/board.h"
#include "periph/rtt.h"
#include "timex.h"
#include "xtimer.h"
#include "periph/uart.h"
#include "isrpipe.h"
#include "periph/gpio.h"

#define GPS_SW_PIN           GPIO_PIN(PORT_F,9)
#define GPS_RESET_PIN        GPIO_PIN(PORT_F,7)
#define GPS_SYNC_1HZ_IN_PIN   GPIO_PIN(PORT_D,3)
#define GPS_SYNC_1HZ_OUT_PIN  GPIO_PIN(PORT_D,6)
#define UART_GPS_DEV           UART_DEV(2)
#define UART_GPS_BAUDRATE      (9600)

#define GPS_RX_BUFSIZE    (512)
static char _rx_buf_mem[GPS_RX_BUFSIZE];
isrpipe_t uart_gps_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

/**********************************************
 * launch_pulse_signal
 * 产生一个脉冲给fpga，为上升沿有效
 * ********************************************/
void launch_pulse_signal(int first)
{
    if(first == 0)  return;

    gpio_write(GPS_SYNC_1HZ_OUT_PIN, 0);
    gpio_write(GPS_SYNC_1HZ_OUT_PIN, 1);
    gpio_write(GPS_SYNC_1HZ_OUT_PIN, 0);
}

extern void GPS_EXIT3_IRQHandler(void *arg);
void gps_sync_1hz_init(void)
{
    gpio_set(GPS_SYNC_1HZ_OUT_PIN);
    gpio_init(GPS_SYNC_1HZ_OUT_PIN, GPIO_IN);
    gpio_init_int(GPS_SYNC_1HZ_IN_PIN, GPIO_IN, GPIO_RISING, GPS_EXIT3_IRQHandler, NULL);
//    NVIC_ClearPendingIRQ(EXTI3_IRQn);
 //   NVIC_DisableIRQ(EXTI3_IRQn);
}

void gps_init(void) {
   ext_pm_power_on_gps();

   gpio_init(GPS_RESET_PIN, GPIO_OUT);
   gpio_set(GPS_RESET_PIN);

   uart_init(UART_GPS_DEV, UART_GPS_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_gps_isrpipe);
   NVIC_ClearPendingIRQ(USART3_IRQn);
   NVIC_DisableIRQ(USART3_IRQn);
   gps_sync_1hz_init();
}

int gps_read(char* buffer, int count)
{
   (void) count;
   return isrpipe_read(&uart_gps_isrpipe, buffer, GPS_RX_BUFSIZE);
}

void gps_enable(void)
{
   tsrb_init(&(uart_gps_isrpipe.tsrb), _rx_buf_mem, GPS_RX_BUFSIZE);
   NVIC_ClearPendingIRQ(USART3_IRQn);
   NVIC_EnableIRQ(USART3_IRQn);
}

void gps_disable(void)
{
   NVIC_ClearPendingIRQ(USART3_IRQn);
   NVIC_DisableIRQ(USART3_IRQn);
}
