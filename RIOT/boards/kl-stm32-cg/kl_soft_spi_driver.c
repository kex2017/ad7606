/*
 * kl_soft_spi_driver.c
 *
 *  Created on: Nov 24, 2017
 *      Author: chenzy
 */

#include "../kl-stm32-cg/include/board.h"
#include "cpu_conf.h"
#include "soft_spi.h"

#include "xtimer.h"

#define KLSTM32_SOFT_SPI_CS   GPIO_PIN(PORT_B, 6)
static soft_spi_t kl_soft_spi = SOFT_SPI_DEV(0);
static soft_spi_cs_t cs = KLSTM32_SOFT_SPI_CS;

int init_kl_soft_spi(void)
{
    /* Initialize software SPI device  */
    soft_spi_init(kl_soft_spi);

    /* Initialize CS pin */
    int tmp = soft_spi_init_cs(kl_soft_spi, cs);
    if (tmp != SOFT_SPI_OK) {
        printf("error: unable to initialize the given chip select line %i\n", tmp);
        return 1;
    }

    printf("init done");
    return 0;
}



#define LSB_MASK                    ((uint16_t)0xFFFF)
#define DBGAFR_POSITION_MASK        ((uint32_t)0x000F0000)
#define DBGAFR_SWJCFG_MASK          ((uint32_t)0xF0FFFFFF)
#define DBGAFR_LOCATION_MASK        ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK         ((uint32_t)0x00100000)

#define GPIO_Remap_SWJ_Disable      ((uint32_t)0x00300400)  /*!< Full SWJ Disabled (JTAG-DP + SW-DP) */
//#define AFIO_MAPR_SWJ_CFG_RESET              ((uint32_t)0x00000000)        /*!< Full SWJ (JTAG-DP + SW-DP) : Reset State */
//#define AFIO_MAPR_SWJ_CFG_NOJNTRST           ((uint32_t)0x01000000)        /*!< Full SWJ (JTAG-DP + SW-DP) but without JNTRST */
//#define AFIO_MAPR_SWJ_CFG_JTAGDISABLE        ((uint32_t)0x02000000)        /*!< JTAG-DP Disabled and SW-DP Enabled */
//#define AFIO_MAPR_SWJ_CFG_DISABLE            ((uint32_t)0x04000000)        /*!< JTAG-DP Disabled and SW-DP Disabled */

void gpio_pin_remap_config(gpio_t gpio_remap, FunctionalState new_status)
{
    uint32_t tmp = 0x00, tmp1 = 0x00, tmpreg = 0x00, tmpmask = 0x00;
//    assert_param(IS_GPIO_REMAP(gpio_remap));
//    assert_param(IS_FUNCTIONAL_STATE(new_status));
    if((gpio_remap & 0x80000000) == 0x80000000)
    {
      tmpreg = AFIO->MAPR2;
    }
    else
    {
//        here
      tmpreg = AFIO->MAPR;
    }

    tmpmask = (gpio_remap & DBGAFR_POSITION_MASK) >> 0x10;
    tmp = gpio_remap & LSB_MASK;

    if ((gpio_remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK))
    {
//        here
      tmpreg &= DBGAFR_SWJCFG_MASK;
      AFIO->MAPR &= DBGAFR_SWJCFG_MASK;
    }
    else if ((gpio_remap & DBGAFR_NUMBITS_MASK) == DBGAFR_NUMBITS_MASK)
    {
      tmp1 = ((uint32_t)0x03) << tmpmask;
      tmpreg &= ~tmp1;
      tmpreg |= ~DBGAFR_SWJCFG_MASK;
    }
    else
    {
      tmpreg &= ~(tmp << ((gpio_remap >> 0x15)*0x10));
      tmpreg |= ~DBGAFR_SWJCFG_MASK;
    }

    if (new_status != DISABLE)
    {
      tmpreg |= (tmp << ((gpio_remap >> 0x15)*0x10));
    }

    if((gpio_remap & 0x80000000) == 0x80000000)
    {
      AFIO->MAPR2 = tmpreg;
    }
    else
    {
      AFIO->MAPR = tmpreg;
    }

}

void kl_soft_spi_write(uint8_t data)
{
    puts("Send 0xaa in SOFT_SPI_MODE_0 with 400KHZ speed");
//    gpio_pin_remap_config(GPIO_Remap_SWJ_Disable, ENABLE);

    soft_spi_acquire(kl_soft_spi, cs, SOFT_SPI_MODE_1, SOFT_SPI_CLK_400KHZ);
    soft_spi_transfer_byte(kl_soft_spi, cs, false, data);
    soft_spi_release(kl_soft_spi);

//    gpio_pin_remap_config(GPIO_Remap_SWJ_Disable, DISABLE);
    puts("Soft SPI Test End");
}
