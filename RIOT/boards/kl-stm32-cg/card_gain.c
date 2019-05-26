#include "../kl-stm32-cg/include/board.h"
#include "periph/gpio.h"

/* 74hc595 pins */
#define  JS_GAIN_DATA_PIN     GPIO_PIN(PORT_G,8)
#define  JS_GAIN_STCP_PIN     GPIO_PIN(PORT_G,6) //W_CP
#define  JS_GAIN_SHCP_PIN     GPIO_PIN(PORT_G,7)

void gain_ctrl_init(void)
{
   gpio_init(JS_GAIN_DATA_PIN, GPIO_OUT);
   gpio_init(JS_GAIN_STCP_PIN, GPIO_OUT);
   gpio_init(JS_GAIN_SHCP_PIN, GPIO_OUT);
}

