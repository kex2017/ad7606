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

void daq_test_fsmc_main(void);

#ifdef __cplusplus
}
#endif

#endif /* KLDAQ_FPGA_FSMC_H_ */
