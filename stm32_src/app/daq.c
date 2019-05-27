#include <errno.h>

#include "log.h"
#include "mutex.h"
#include "xtimer.h"

#include "daq.h"
#include "kldaq.h"
#include "kldaq_fpga_spi.h"

/*read reg define*/
#define DAT_MAX_REG (4)
#define LTC_NUM_REG_H (8)
#define LTC_NUM_REG_L (9)
#define CLK_CNT_REG_H (10)
#define CLK_CNT_REG_L (11)
#define DISCHARGE_NUM_REG_H (14)
#define DISCHARGE_NUM_REG_L (15)

/*write reg define*/
#define PD_TW_THRE_REG (1)
#define PD_TW_CHANGE_THRE_REG (6)
#define DATA_DONE_FLAG_REG (7)
#define CFG_CHIP1_REG (9)
#define CFG_CHIP2_REG (10)

//kldaq_dev_t* g_spi_dev = (kldaq_dev_t*)&fpga_mock_with_ps_cnf_dev;
kldaq_dev_t* g_spi_dev = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;  //fpga_spi_mock_with_ps_cnf_dev

static mutex_t spi_fpga_ops_mtx;

void acquire_spi_ops_mtx(void)
{
    mutex_lock(&spi_fpga_ops_mtx);
}
void release_spi_ops_mtx(void)
{
    mutex_unlock(&spi_fpga_ops_mtx);
}

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
//    check_spi_fpga_work_status(g_spi_dev);
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

int daq_spi_get_dat_max(uint8_t channel)
{
    return daq_spi_read_reg(channel, DAT_MAX_REG);
}

uint16_t daq_spi_get_discharge_num(uint8_t channel)
{
    uint16_t discharge_num = 0;

    discharge_num = daq_spi_read_reg(channel, DISCHARGE_NUM_REG_L);

    return discharge_num;
}

int daq_spi_clear_data_done_flag(uint8_t channel)
{
    uint16_t done_reg = daq_spi_read_reg(0, DATA_DONE_FLAG_REG);
    uint16_t temp1 = (1 << channel) | done_reg;
    daq_spi_write_reg(0, DATA_DONE_FLAG_REG, temp1);
    uint16_t temp2 =  ~(1 << channel) & done_reg;
    daq_spi_write_reg(0, DATA_DONE_FLAG_REG,temp2);
    return 0;
}

uint32_t daq_spi_one_sec_clk_cnt(void)
{
    uint32_t clk_cnt = 0;

    clk_cnt = daq_spi_read_reg(0, CLK_CNT_REG_H) << 16 | daq_spi_read_reg(0, CLK_CNT_REG_L);

    return clk_cnt;
}

uint32_t daq_spi_ltc_num_since_plus(void)
{
    uint32_t ltc_num = 0;

    ltc_num = daq_spi_read_reg(0, LTC_NUM_REG_H) << 16 | daq_spi_read_reg(0, LTC_NUM_REG_L);

    return ltc_num;
}

int daq_spi_start_sample(void)
{
    kldaq_trigger_one_sample(g_spi_dev, 0);
    return 0;
}

int daq_spi_sample_done_check(uint8_t channel)
{
    return kldaq_is_sample_done(g_spi_dev, channel);
}

uint32_t daq_spi_sample_len_get(uint8_t channel)
{
    return kldaq_get_length(g_spi_dev, channel);
}

int daq_spi_set_pd_tw_threshold(uint8_t channel, uint16_t threshold)
{
    daq_spi_write_reg(channel, PD_TW_THRE_REG, threshold);
    return 0;
}

int daq_spi_set_pd_tw_change_threshold(uint8_t channel, uint16_t threshold)
{
    daq_spi_write_reg(channel, PD_TW_CHANGE_THRE_REG, threshold);
    return 0;
}

int daq_spi_cfg_chip_data(uint8_t channel, uint8_t chip_no, uint16_t data)
{
    uint16_t reg = 0;
    if (1 == chip_no) {
        reg = CFG_CHIP1_REG;
    }
    else if (2 == chip_no) {
        reg = CFG_CHIP2_REG;
    }
    daq_spi_write_reg(channel, reg, (1 << 14) | (data & 0xFFF));
    return 0;
}

int daq_spi_cfg_chip_is_done(uint8_t channel, uint8_t chip_no)
{
    uint16_t done_flag = 0, reg = 0;
    if (1 == chip_no) {
        reg = CFG_CHIP1_REG;
    }
    else if (2 == chip_no) {
        reg = CFG_CHIP2_REG;
    }
    done_flag = daq_spi_read_reg(channel, reg);
    return (done_flag >> 15) & 0x01;
}

int daq_spi_cfg_chip_clear_done_flag(uint8_t channel, uint8_t chip_no)
{
    uint16_t reg = 0;
    if (1 == chip_no) {
        reg = CFG_CHIP1_REG;
    }
    else if (2 == chip_no) {
        reg = CFG_CHIP2_REG;
    }
    daq_spi_write_reg(channel, reg, 0);
    return 0;
}

