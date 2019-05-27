
#ifndef KLDAQ_FPGA_SPI_H_
#define KLDAQ_FPGA_SPI_H_

#include <stdint.h>

#include "periph_conf.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "kldaq.h"
#include "klfpga.h"

#ifdef __cplusplus
extern "C"
{
#endif

//use bit4~bit7 as command code, and bit0~bit3 as channel number
typedef struct {
    uint8_t rreg; /*read register*/
    uint8_t rdat; /*read data*/
    uint8_t wreg; /*write register*/
} daq_opcode_t;


//extern const kldaq_desc_t kldaq_fpga_spi_driver;
//extern const daq_opcode_t fpga_spi_opcode_default;

typedef struct fpga_spi_conf{
    kldaq_dev_t base;
    spi_t spi;
    const daq_opcode_t* opcode;
    gpio_t cs_pin;           /**< SPI CS pin */
    spi_mode_t mode;         /**< SPI mode */
    spi_clk_t clk;           /**< SPI clock */
} fpga_spi_dev_t;

extern fpga_spi_dev_t fpga_spi_with_ps_cnf_dev;
extern fpga_spi_dev_t fpga_spi_with_dummy_cnf_dev;

void daq_test_spi_main(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* KLDAQ_FPGA_SPI_H_ */
