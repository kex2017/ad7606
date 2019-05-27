#include "stdio.h"
#include "periph/gpio.h"
#include "switch_value.h"
void switch_init(void)
{
     gpio_init(SW_1, GPIO_IN);
     gpio_init(SW_2, GPIO_IN);
     gpio_init(SW_3, GPIO_IN);
     gpio_init(SW_4, GPIO_IN);
     gpio_init(SW_5, GPIO_IN);
}

uint8_t read_sw(uint8_t sw_no)
{
    switch (sw_no) {
    case 1:
        return !!gpio_read(SW_1);
    case 2:
        return !!gpio_read(SW_2);
    case 3:
        return !!gpio_read(SW_3);
    case 4:
        return !!gpio_read(SW_4);
    case 5:
        return !!gpio_read(SW_5);
    default:
        return 0;
    }
}
