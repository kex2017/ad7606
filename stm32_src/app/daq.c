#include <errno.h>

#include "log.h"
#include "mutex.h"
#include "xtimer.h"

#include "daq.h"
#include "kldaq.h"
#include "kldaq_fpga_spi.h"

kldaq_dev_t* g_spi_dev = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;

void check_spi_fpga_work_status(kldaq_dev_t* dev)
{
    if (kldaq_is_microcode_work_ok(dev)) {
        printf("Get SPI FPGA work status as: OK!\r\n");
    }
    else {
        printf("Get SPI FPGA work status as: NOK!!\r\n");
    }
}

void daq_init(void)
{
    kldaq_init(g_spi_dev);
    check_spi_fpga_work_status(g_spi_dev);
}

void set_sample_curve_ops(kldaq_dev_t *dev)
{
    g_spi_dev = dev;
    daq_init();
}

/*****************************************************************************
 * SPI FPGA OPS
 *****************************************************************************/
int daq_spi_read_reg(uint8_t channel, uint16_t reg)
{
    return kldaq_read_reg(g_spi_dev, channel, reg);
}

int daq_spi_write_reg(uint8_t channel, uint16_t reg, uint16_t value)
{
    kldaq_write_reg(g_spi_dev, channel, reg, value);
    return 0;
}

int daq_spi_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    return kldaq_read_data(g_spi_dev, channel, data, addr, byte_len);
}

/**************************************************************************/
int daq_spi_sample_done_check(uint8_t channel)
{
    uint16_t chan_status = daq_spi_read_reg(channel, CHA_STA_REG);
    return chan_status&0x0002;
}

uint16_t daq_spi_get_data_len(uint8_t channel)
{
    (void) channel;
    return CURVE_LEN;
}

int daq_spi_get_dat_max(uint8_t channel)
{
    return daq_spi_read_reg(channel, DAT_MAX_REG);
}

int daq_spi_get_dat_avr(uint8_t channel)
{
    return daq_spi_read_reg(channel, DAT_AVR_REG);
}

uint32_t daq_spi_one_sec_clk_cnt(void)
{
    uint32_t clk_cnt = 0;

    clk_cnt = daq_spi_read_reg(0, ONE_SECOND_CNT_H) << 16 | daq_spi_read_reg(0, ONE_SECOND_CNT_L);

    return clk_cnt;
}

uint32_t daq_spi_chan_cnt_since_plus(uint8_t chan_no)
{
    uint32_t chan_cnt = 0;

    if(chan_no == 0){
        chan_cnt = daq_spi_read_reg(0, CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H) << 16 | daq_spi_read_reg(0, CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L);
    }
    else if(chan_no == 1){
        chan_cnt = daq_spi_read_reg(0, CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H) << 16 | daq_spi_read_reg(0, CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L);
    }

    return chan_cnt;
}

void daq_spi_read_test_reg(void)
{
    uint16_t test_reg_fix = 0x1357;
    uint16_t read_value = 0;

    read_value = daq_spi_read_reg(0, FPGA_READ_TEST_REG);
    if(read_value == test_reg_fix){
        LOG_INFO("succeed!!!  read value is %0x%04x\r\n", read_value);
    }
    else{
        LOG_ERROR("failed!!!  read value is %0x%04x\r\n", read_value);
    }
}

/*write ops*/
int daq_spi_clear_data_done_flag(uint8_t channel)
{
    if(channel == 0){
        daq_spi_write_reg(0, CHA_RUN_REG, 1 << 4);
    }
    else if(channel == 1){
        daq_spi_write_reg(0, CHA_RUN_REG, 1 << 5);
    }
    return 0;
}

int daq_spi_set_threshold(uint8_t channel, uint16_t threshold)
{
    daq_spi_write_reg(channel, DAT_THR_REG, threshold);
    return 0;
}

int daq_spi_set_change_rate(uint8_t channel, uint16_t changerate)
{
    daq_spi_write_reg(channel, CHANGE_RATE_THR, changerate);
    return 0;
}
