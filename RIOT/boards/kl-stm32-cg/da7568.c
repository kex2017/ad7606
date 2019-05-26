#include "periph/gpio.h"
#include "timex.h"
#include "xtimer.h"

/* dac7568 pins */
#define  DAC7568_SCK       GPIO_PIN(PORT_G,6)
#define  DAC7568_DIN       GPIO_PIN(PORT_G,8)
#define  DAC7568_SYNC      GPIO_PIN(PORT_G,7)

#define DA7568_SYNC_H   gpio_set(DAC7568_SYNC)
#define DA7568_SYNC_L   gpio_clear(DAC7568_SYNC)
#define DA7568_DIN_H    gpio_set(DAC7568_DIN)
#define DA7568_DIN_L    gpio_clear(DAC7568_DIN)
#define DA7568_SCLK_H   gpio_set(DAC7568_SCK)
#define DA7568_SCLK_L   gpio_clear(DAC7568_SCK)

void write_7568_data(unsigned long int data)
{
     unsigned char i;
     xtimer_ticks32_t last_wakeup = xtimer_now();


     DA7568_SYNC_H;
     DA7568_DIN_L;
     DA7568_SCLK_L;
     xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000/1000);
     DA7568_SYNC_L;
     for(i=0;i<32;i++)
     {
       DA7568_SCLK_H;//@Cui Fuxing时钟上升沿的过程送数据
             if((data&0x80000000)==0x80000000)
             {
                DA7568_DIN_H;
             }
             else
             {
                DA7568_DIN_L;
             }
             DA7568_SCLK_L;
             data<<=1;
     }
     DA7568_SYNC_H;
}

void hal_init_da7568(void)
{
    gpio_init(DAC7568_SCK, GPIO_OUT);
    gpio_init(DAC7568_DIN, GPIO_OUT);
    gpio_init(DAC7568_SYNC, GPIO_OUT);
    write_7568_data(0x090a0000); //启用内部参考
// write_7568_data(0x090c0000);//启用外部参考
}

void da7568_out(uint8_t channel, uint16_t value)
{
    uint32_t data;
    data = 0x03000000 | ((uint32_t)channel) << 20 | ((uint32_t)value) << 8;
    write_7568_data(data);
}

#define CONVERT_GAIN_FROM_DB_TO_V(x) ((x - 5.5) / 50)
void hal_JF_gain_out(uint8_t channel, float value)
{
    value = CONVERT_GAIN_FROM_DB_TO_V(value) > 1.0 ? 1.0 : CONVERT_GAIN_FROM_DB_TO_V(value);
    da7568_out(channel, (uint16_t)(value/2.5*4096));
}
