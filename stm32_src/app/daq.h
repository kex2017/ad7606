#ifndef DAQ_H_
#define DAQ_H_

#include <stdint.h>
#include <errno.h>

#include "kldaq.h"
void acquire_spi_ops_mtx(void);
void release_spi_ops_mtx(void);

void daq_init(void);
void set_sample_curve_ops(kldaq_dev_t *dev);

//SPI FPGA DAQ interface
int daq_spi_read_reg(uint8_t channel, uint16_t reg);
int daq_spi_write_reg(uint8_t channel, uint16_t reg, uint16_t value);
int daq_spi_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len);
//峰值
int daq_spi_get_dat_max(uint8_t channel);
//放电次数
uint16_t daq_spi_get_discharge_num(uint8_t channel);
//秒脉冲计数
uint32_t daq_spi_one_sec_clk_cnt(void);
//局放/行波越限 距秒脉冲计数
uint32_t daq_spi_ltc_num_since_plus(void);
//采集开始控制
int daq_spi_start_sample(void);
//采集完成检查
int daq_spi_sample_done_check(uint8_t channel);
//采集数据长度
uint32_t daq_spi_sample_len_get(uint8_t channel);
//行波&局放门限设置
int daq_spi_set_pd_tw_threshold(uint8_t channel, uint16_t threshold);
//行波&局放变化率门限设置
int daq_spi_set_pd_tw_change_threshold(uint8_t channel, uint16_t threshold);
//BGA3031-chip1 chip2 配置数据及使能
int daq_spi_cfg_chip_data(uint8_t channel, uint8_t chip_no, uint16_t data);
//判断chip1，chip2配置是否完成
int daq_spi_cfg_chip_is_done(uint8_t channel, uint8_t chip_no);
//清零chip1,chip2配置完成位
int daq_spi_cfg_chip_clear_done_flag(uint8_t channel, uint8_t chip_no);
//清数据读取标志
int daq_spi_clear_data_done_flag(uint8_t channel);

#endif

