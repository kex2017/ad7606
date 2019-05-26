#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "../../kl-stm32-cg/include/periph_conf.h"
#include "cpu.h"
#include "mtd.h"

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

/**
 * @name    Board pin definitions and handlers
 * @{
 */
/* Watch Dog Feed pin*/
#define DOG_PIN            GPIO_PIN(PORT_A, 8)
#define DOG_MASK           (1 << 8)
#define DOG_ON             (GPIOA->ODR &= ~DOG_MASK)
#define DOG_OFF            (GPIOA->ODR |=  DOG_MASK)
void watch_dog_init(void);

void IWDG_Feed(void);
void IWDG_init(void);
/**
 * @ GPS Module handler
 */
void gps_init(void);
int gps_read(char* buffer, int count);
void gps_enable(void);
void gps_disable(void);
void launch_pulse_signal(int first);

/**
 * External Device Power Management
 */

void ext_pm_ctrl_init(void);
void ext_pm_power_on_all(void);
void reset_ext_fpga_power(void);

void ext_pm_power_on_gps(void);
void ext_pm_power_off_gps(void);
void ext_pm_power_on_jf(void);
void ext_pm_power_off_jf(void);
void ext_pm_power_on_gy(void);
void ext_pm_power_off_gy(void);


/**
 * Gain Control
 */
void gain_ctrl_init(void);

/**
 * over voltage warning
 */
void over_voltage_event_pins_init(void);
void enable_over_voltage_irq(void);
void disable_over_voltage_irq(void);


/**
 * partial discharge warning
 */
void partial_discharge_event_pins_init(void);
void enable_partial_discharge_irq(void);
void disable_partial_discharge_irq(void);

/**
 * FSMC Module
 */
#define     FPGA_BASE_ADDR            0x60000000
#define     SRAM_BASE_ADDR            0x64000000
#define     DM9000_BASE_ADDR          0x6C000000

//fpga

/* fpga cfg pins */
#define  FPGA_nCFG_PIN     GPIO_PIN(PORT_C,2)
#define  FPGA_DCLK_PIN     GPIO_PIN(PORT_C,3)
#define  FPGA_DATA0_PIN     GPIO_PIN(PORT_F,11)
#define  FPGA_nSTS_PIN     GPIO_PIN(PORT_C,0)
#define  FPGA_DONE_PIN     GPIO_PIN(PORT_C,1)


/** @} */


/**
 *
 */
void spi_flash_test(void);
void mtd_flash_test(void);

int init_kl_soft_spi(void);
void kl_soft_spi_write(uint8_t data);

/**
 * @name Kl-stm32 NOR flash hardware configuration
 */
/** @{ */

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

/** @} */
/**
 * @name MTD configuration
 */
/** @{ */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
/** @} */

/** @} */
/**
 * @name da7568
 */
/** @{ */
void hal_init_da7568(void);
void hal_JF_gain_out(uint8_t channel, float value);
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
