#ifndef BOARD_H
#define BOARD_H


#include "../../kl-cg-ibox-new/include/periph_conf.h"
#include "cpu.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Xtimer configuration
 *
 * Tell the xtimer that we use a 16-bit peripheral timer
 * @{
 */
#define XTIMER_WIDTH        (16)
/** @} */

void ext_pm_ctrl_init(void);
void ext_pm_power_on_all(void);
/* fpga cfg pins */
#define  FPGA_nCFG_PIN     (GPIO_PIN(PORT_F,14))
#define  FPGA_DCLK_PIN     (GPIO_PIN(PORT_F,15))
#define  FPGA_DATA0_PIN    (GPIO_PIN(PORT_F,11))
#define  FPGA_nSTS_PIN     (GPIO_PIN(PORT_F,12))
#define  FPGA_DONE_PIN     (GPIO_PIN(PORT_F,13))


#define KLSTM32_NOR_SPI_DEV               SPI_DEV(0)
#define KLSTM32_NOR_SPI_CLK               SPI_CLK_5MHZ
#define KLSTM32_NOR_SPI_MODE              SPI_MODE_3
#define KLSTM32_NOR_SPI_CS                GPIO_PIN(PORT_B, 12) /**< Flash CS pin */

/*overwrite the sdcard spi conf in sdcard_spi_para.h*/
#define SDCARD_SPI_PARAM_SPI         (SPI_DEV(0))
#define SDCARD_SPI_PARAM_CS          (GPIO_PIN(PORT_C, 8)) /*spi sd card*/
#define SDCARD_SPI_PARAM_CLK         (GPIO_PIN(PORT_B, 13))
#define SDCARD_SPI_PARAM_MOSI        (GPIO_PIN(PORT_B, 15))
#define SDCARD_SPI_PARAM_MISO        (GPIO_PIN(PORT_B, 14))

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
