#ifndef DAQ_H_
#define DAQ_H_

#include <stdint.h>
#include <errno.h>

#include "kldaq.h"

typedef enum{
    FPGA_A_CS = 0,
    FPGA_B_CS,
    FPGA_C_CS
}fpga_cs_t;

#define SPI1_A_CS             GPIO_PIN(PORT_E, 9)
#define SPI1_B_CS             GPIO_PIN(PORT_G, 2)
#define SPI1_C_CS             GPIO_PIN(PORT_C, 7)

#define HF_CHANNEL_1_NUM (0)
#define HF_CHANNEL_2_NUM (1)

#define PF_CHANNEL_1_NUM (2)
#define PF_CHANNEL_2_NUM (3)

/*read reg define*/
#define CHA_STA_REG 1

#define DAT_LEN_H 2
#define DAT_LEN_L 3

#define DAT_MAX_REG 4
#define DAT_AVR_REG 5

#define CHA0_ONE_SECOND_CNT_H 8
#define CHA0_ONE_SECOND_CNT_L 9

#define CHA1_ONE_SECOND_CNT_H 24
#define CHA1_ONE_SECOND_CNT_L 25

#define CHA2_ONE_SECOND_CNT_H 0x32
#define CHA2_ONE_SECOND_CNT_L 0x33

#define CHA3_ONE_SECOND_CNT_H 0x34
#define CHA3_ONE_SECOND_CNT_L 0x35

#define CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H 10
#define CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L 11

#define CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H 22
#define CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L 23

#define CHA0_EVENT_UTC_TIME_H 18
#define CHA0_EVENT_UTC_TIME_L 19

#define CHA1_EVENT_UTC_TIME_H 20
#define CHA1_EVENT_UTC_TIME_L 21

#define CHA2_EVENT_UTC_TIME_H 0x2D
#define CHA2_EVENT_UTC_TIME_L 0x2E

#define CHA3_EVENT_UTC_TIME_H 0x30
#define CHA3_EVENT_UTC_TIME_L 0x31

#define SET_UTC_TIME_H 19
#define SET_UTC_TIME_L 20

#define LTC_OCR_REG 0x1F //发生超越门限，并且数据准备好
#define LTC_LENGTH_REG 0x21 //数据长度寄存器

#define LTC_OTH_CHA_REG_L 0x46 //门限寄存器
#define LTC_OTH_CHA_REG_M 0x47
#define LTC_OTH_CHA_REG_H 0x48

#define LTC_OTH_CHB_REG_L 0x49
#define LTC_OTH_CHB_REG_M 0x4A
#define LTC_OTH_CHB_REG_H 0x4B

#define LTC_GP_SQRT_CNTA_H 0x23 //工频A通道平方和数据，为高16bit
#define LTC_GP_SQRT_CNTA_M 0x24
#define LTC_GP_SQRT_CNTA_L 0x25

#define LTC_GP_SQRT_CNTB_H 0x26 //工频B通道平方和数据，为高16bit
#define LTC_GP_SQRT_CNTB_M 0x27
#define LTC_GP_SQRT_CNTB_L 0x28

#define LTC_GP_PLS_CNTA_H 0x29 //工频A通道秒脉冲数据，为高16bit
#define LTC_GP_PLS_CNTA_L 0x2A

#define LTC_GP_PLS_CNTB_H 0x2B //工频B通道秒脉冲数据，为高16bit
#define LTC_GP_PLS_CNTB_L 0x2C

#define LTC_FINSH_CHB_REG_H 0x4C // 读完成寄存器，bit1,bit0置1 表示读完成

#define FPGA_READ_TEST_REG 0x22

#define CURVE_LEN 4096

/*write reg define*/
#define CHA_RUN_REG 0
#define DAT_THR_REG 1
#define CHANGE_RATE_THR 6
#define TRIGGER_REG 7

void change_spi_cs_pin(fpga_cs_t cs_no);
void change_spi_cs_pin_release(void);
fpga_cs_t get_cur_fpga_cs(void);

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
//读取fpga utc 时间
uint32_t daq_spi_chan_event_utc(uint8_t chan_no);
//设置fpga utc时间
void daq_spi_chan_set_fpga_utc(uint32_t utc_time);
//秒脉冲计数
uint32_t daq_spi_one_sec_clk_cnt(uint8_t chan_no);
//通道故障原始波形距离秒脉冲的时钟计数
uint32_t daq_spi_chan_cnt_since_plus(uint8_t chan_no);
//读取FPGA给定特定数值的寄存器（用于验证与fpga通信）
void daq_spi_read_test_reg(void);
//触发FPGA无条件采集波形
void daq_spi_trigger_sample(uint8_t channel);
//门限设置
int daq_spi_set_hf_threshold(uint8_t channel, uint16_t threshold);
//变化率设置
int daq_spi_set_hf_change_rate(uint8_t channel, uint16_t changerate);
//清除高频通道数据有效指令
int daq_spi_clear_data_done_flag(uint8_t channel);

int daq_spi_set_pf_threshold(uint8_t channel, uint64_t  threshold);

uint64_t daq_spi_get_pf_sum_data(uint8_t channel);
#endif

