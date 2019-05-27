#ifndef DAQ_H_
#define DAQ_H_

#include <stdint.h>
#include <errno.h>

#include "kldaq.h"

/*read reg define*/
#define CHA_STA_REG 1

#define DAT_LEN_H 2
#define DAT_LEN_L 3

#define DAT_MAX_REG 4
#define DAT_AVR_REG 5

#define ONE_SECOND_CNT_H 8
#define ONE_SECOND_CNT_L 9

#define CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H 10
#define CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L 11

#define CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H 12
#define CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L 13

#define FPGA_READ_TEST_REG 14

#define CURVE_LEN 4096

/*write reg define*/
#define CHA_RUN_REG 0
#define DAT_THR_REG 1
#define CHANGE_RATE_THR 6

void daq_init(void);
void set_sample_curve_ops(kldaq_dev_t *dev);
void check_spi_fpga_work_status(kldaq_dev_t* dev);

//SPI FPGA DAQ interface
int daq_spi_read_reg(uint8_t channel, uint16_t reg);
int daq_spi_write_reg(uint8_t channel, uint16_t reg, uint16_t value);
int daq_spi_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len);
//读取采集长度
uint16_t daq_spi_get_data_len(uint8_t channel);
//均值
int daq_spi_get_dat_avr(uint8_t channel);
//峰值
int daq_spi_get_dat_max(uint8_t channel);
//检测该通道是否抓取到异常波形
int daq_spi_sample_done_check(uint8_t channel);
//秒脉冲计数
uint32_t daq_spi_one_sec_clk_cnt(void);
//通道故障原始波形距离秒脉冲的时钟计数
uint32_t daq_spi_chan_cnt_since_plus(uint8_t chan_no);
//读取FPGA给定特定数值的寄存器（用于验证与fpga通信）
void daq_spi_read_test_reg(void);
//门限设置
int daq_spi_set_threshold(uint8_t channel, uint16_t threshold);
//变化率设置
int daq_spi_set_change_rate(uint8_t channel, uint16_t changerate);
//清除高频通道数据有效指令
int daq_spi_clear_data_done_flag(uint8_t channel);
#endif

