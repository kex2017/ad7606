#ifndef DAQ_H_
#define DAQ_H_

#include <stdint.h>
#include <errno.h>

#include "kldaq.h"

void acquire_fsmc_ops_mtx(void);
void release_fsmc_ops_mtx(void);
void acquire_spi_ops_mtx(void);
void release_spi_ops_mtx(void);

void daq_init(void);
void set_sample_curve_ops(kldaq_dev_t *dev);
void set_sample_curve_with_mock_ops(void);
void set_sample_curve_with_real_ops(void);


//FSMC FPGA DAQ interface
uint16_t daq_fsmc_read_reg(uint16_t reg);
int daq_fsmc_write_reg(uint16_t reg, uint16_t value);
int daq_fsmc_start_sample(void);
int daq_fsmc_single_chan_sample(uint8_t channel);
int daq_fsmc_multi_chans_sample(uint8_t *channels, size_t count);
int daq_fsmc_sample_done_check(uint8_t channel);
uint32_t daq_fsmc_sample_len_get(uint8_t channel);
int daq_fsmc_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len);
uint16_t daq_fsmc_curve_max_value_get(uint8_t channel);
int daq_fsmc_over_threshold_check(uint8_t channel);
int daq_fsmc_channel_threshold_set(uint8_t channel, uint16_t threshold);
int daq_fsmc_channel_changerate_set(uint8_t channel, uint16_t change_rate);
int daq_fsmc_sync_type_set(uint8_t sync_type);
int daq_fsmc_noise_channel_set(uint8_t channel, uint8_t noise_flag);
int daq_fsmc_channel_cfg_set(uint8_t channel, uint16_t start_fre, uint16_t end_fre, uint16_t threshold, uint16_t change_rate, uint8_t mode);
int daq_fsmc_channel_pd_cfg_set(uint8_t channel, uint16_t threshold, uint16_t change_rate);
uint32_t daq_fsmc_pd_ns_get(uint8_t channel);
int daq_fsmc_pd_seconds_get(uint32_t *timestamp);
int daq_fsmc_pd_flag_reset(uint16_t channels);
int daq_fsmc_pd_work_mode_set(uint8_t mode);
int daq_fsmc_fpga_version_get(uint16_t *fpga_version);
int daq_fsmc_fpga_time_set(uint32_t timestamp);
void daq_fsmc_fpga_power_reset(void);
int daq_fsmc_prpd_sample_status_set(uint8_t status);
int reset_channel_sample(uint8_t channel);


//SPI FPGA DAQ interface
int daq_spi_sample_done_check(uint8_t channel);
uint32_t daq_spi_sample_len_get(uint8_t channel);
int daq_spi_fpga_version_read(uint16_t *fpga_version);
int daq_spi_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len);
int daq_spi_curve_max_get(uint8_t channel);
int daq_spi_channel_threshold_set(uint8_t channel, uint16_t threshold);
int daq_spi_channel_changerate_set(uint8_t channel, uint16_t changerate);
int daq_spi_ov_flag_reset(uint8_t channel);



#endif
