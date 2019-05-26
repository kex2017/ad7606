#ifndef KLDAQ_FPGA_FSMC_H_
#define KLDAQ_FPGA_FSMC_H_

#include <stdint.h>

#include "periph_conf.h"
#include "periph/gpio.h"
#include "kldaq.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern kldaq_dev_t fpga_fsmc_dev_without_img_download;
extern kldaq_dev_t fpga_fsmc_dev_with_img_download;

uint16_t  fpga_read(uint32_t offset);
void  fpga_write(uint32_t offset, uint16_t data);
void fpga_set_over_voltage_threshold(uint16_t *threshold);
void fpga_start_run(unsigned short channels_mask);

void daq_test_fsmc_main(void);

#ifdef __cplusplus
}
#endif

#endif /* KLDAQ_FPGA_FSMC_H_ */
