#ifndef DAQ_CURVE_OPS_MOCK_H_
#define DAQ_CURVE_OPS_MOCK_H_

#include "kldaq.h"
#include "klfpga.h"

typedef struct fpga_mock_dev{
    kldaq_dev_t base;
} fpga_mock_dev_t;

extern fpga_mock_dev_t fpga_fsmc_mock_with_ps_cnf_dev;
extern fpga_mock_dev_t fpga_spi_mock_with_ps_cnf_dev;

#endif
