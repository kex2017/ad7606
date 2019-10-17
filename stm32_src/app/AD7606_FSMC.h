#ifndef AD7606_FSMC_H_
#define AD7606_FSMC_H_

#include <stdint.h>
#include "xtimer.h"
#include "timex.h"
#include "periph/rtt.h"
#include "periph/gpio.h"
#include "periph/stm32f10x_std_periph.h"

#define OS0_PIN GPIO_PIN(PORT_G, 8)
#define OS1_PIN GPIO_PIN(PORT_G, 7)
#define OS2_PIN GPIO_PIN(PORT_G, 6)

#define RANGE_PIN GPIO_PIN(PORT_G, 5)

#define CNAB_PIN GPIO_PIN(PORT_A, 8)

#define RST_PIN GPIO_PIN(PORT_D, 11)

#define BUSY_PIN GPIO_PIN(PORT_G, 4)

//#define AD7606_RESULT() *(__IO uint16_t *)0x60000000

typedef struct
{
//__IO uint16_t AD7606_Write;
__IO uint16_t AD7606_Read;
}
AD7606_TypeDef;

#define AD7606_BASE        ((uint32_t)(0x60000000))//0x6C000000
#define AD7606_FSMC        ((AD7606_TypeDef *) AD7606_BASE)

#define DATA_LEN 128  //数组深度

typedef struct
{
    short int  CHn[8][DATA_LEN];//8个通道，每个深度 DATA_LEN
    uint16_t  W_index;   //CHn 数组的 索引
}AD7606Para;

void AD7606_Init(void);

void conv_ab(void);

void print_AD7606(void);

uint16_t FSMC_AD7606_RD(void);

#endif
