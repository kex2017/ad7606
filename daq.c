#include <errno.h>

#include "log.h"
#include "mutex.h"
#include "xtimer.h"

#include "daq.h"
#include "daq_curve_ops_mock.h"
#include "kldaq.h"
#include "kldaq_fpga_fsmc.h"
#include "kldaq_fpga_spi.h"

static uint16_t RESET_REG = 7;
#define MAX_COMMOM_FPGA_CHANNEL 13
//kldaq_dev_t* g_fsmc_dev = (kldaq_dev_t*)&fpga_fsmc_dev_with_img_download; //hardware v1
//kldaq_dev_t* g_fsmc_dev = (kldaq_dev_t*)&fpga_fsmc_dev_without_img_download;
//kldaq_dev_t* g_spi_daq = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;
//kldaq_dev_t* g_spi_daq = (kldaq_dev_t*)&fpga_spi_with_dummy_cnf_dev;


kldaq_dev_t* g_fsmc_dev = (kldaq_dev_t*)&fpga_fsmc_dev_without_img_download; //fpga_fsmc_mock_with_ps_cnf_dev
kldaq_dev_t* g_spi_dev = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;  //fpga_spi_mock_with_ps_cnf_dev

static uint16_t CHANNEL_REG = 0;
static uint16_t THRESHOLD_REG = 1;
static uint16_t NOISE_REG = 3;
static uint16_t CHANGERATE_REG = 6;
static int16_t TRIGGER_REG = 8;
static int16_t START_FRE_REG = 10;
static int16_t STOP_FRE_REG = 11;
//多个功能复用, 设置前需先读取原值结合修改, 不涉及的功能位切勿修改
static uint16_t PRPD_STATUS_REG = 15;   //bit0
static uint16_t FILTER_MODE_REG = 15; //bit3~bit6
static uint16_t SYNC_TYPE_REG = 15;   //bit15

static mutex_t fsmc_fpga_ops_mtx, spi_fpga_ops_mtx;
void acquire_fsmc_ops_mtx(void)
{
    mutex_lock(&fsmc_fpga_ops_mtx);
}
void release_fsmc_ops_mtx(void)
{
    mutex_unlock(&fsmc_fpga_ops_mtx);
}

void acquire_spi_ops_mtx(void)
{
    mutex_lock(&spi_fpga_ops_mtx);
}
void release_spi_ops_mtx(void)
{
    mutex_unlock(&spi_fpga_ops_mtx);
}

uint16_t daq_fsmc_read_reg(uint16_t reg)
{
    return kldaq_read_reg(g_fsmc_dev, reg);
}

int daq_fsmc_write_reg(uint16_t reg, uint16_t value)
{
    return kldaq_write_reg(g_fsmc_dev, reg, value);
}

int daq_fsmc_start_sample(void)
{
    return kldaq_write_reg(g_fsmc_dev, TRIGGER_REG, 1);
}

int daq_fsmc_single_chan_sample(uint8_t channel)
{
    kldaq_trigger_one_sample(g_fsmc_dev, channel);
    kldaq_write_reg(g_fsmc_dev, TRIGGER_REG, 1);
    return 0;
}

int daq_fsmc_multi_chans_sample(uint8_t *channels, size_t count)
{
    kldaq_trigger_multi_channels_sample(g_fsmc_dev, channels, count);
    kldaq_write_reg(g_fsmc_dev, TRIGGER_REG, 1);
    return 0;
}

int reset_channel_sample(uint8_t channel)
{
    daq_fsmc_write_reg(0, channel);
    daq_fsmc_write_reg(RESET_REG, 1 << 0);
    return 0;
}
int daq_fsmc_sample_done_check(uint8_t channel)
{
    return kldaq_is_sample_done(g_fsmc_dev, channel);
}

uint32_t daq_fsmc_sample_len_get(uint8_t channel)
{
    return kldaq_get_length(g_fsmc_dev, channel);
}

int daq_fsmc_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    return kldaq_read_data(g_fsmc_dev, channel, data, addr, byte_len);
}

uint16_t daq_fsmc_curve_max_value_get(uint8_t channel)
{
    return kldaq_get_max(g_fsmc_dev, channel);
}

int daq_fsmc_over_threshold_check(uint8_t channel)
{
    return kldaq_is_over_threshold(g_fsmc_dev, channel);
}

int daq_fsmc_channel_threshold_set(uint8_t channel, uint16_t threshold)
{
    return kldaq_set_threshold(g_fsmc_dev, channel, threshold);
}

int daq_fsmc_channel_changerate_set(uint8_t channel, uint16_t change_rate)
{
    return kldaq_set_changerate(g_fsmc_dev, channel, change_rate);
}

int daq_fsmc_sync_type_set(uint8_t sync_type)
{
    uint16_t value;
    value = kldaq_read_reg(g_fsmc_dev, SYNC_TYPE_REG);
    value |= (!!sync_type) << 15;  //bit15表示同步方式, 其余 bit 代表其他功能, 需保持不变
    kldaq_write_reg(g_fsmc_dev, SYNC_TYPE_REG, value);
    return 0;
}

int daq_fsmc_noise_channel_set(uint8_t channel, uint8_t noise_flag)
{
    kldaq_write_reg(g_fsmc_dev, NOISE_REG, (noise_flag ? 0x8000 : 0x0000) | channel);
    return 0;
}

int daq_fsmc_channel_cfg_set(uint8_t channel, uint16_t start_fre, uint16_t end_fre, uint16_t threshold, uint16_t change_rate, uint8_t mode)
{
    kldaq_write_reg(g_fsmc_dev, CHANNEL_REG, channel);          //选择通道
    kldaq_write_reg(g_fsmc_dev, START_FRE_REG, start_fre);      //开始频率
    kldaq_write_reg(g_fsmc_dev, STOP_FRE_REG, end_fre);         //结束频率
    kldaq_write_reg(g_fsmc_dev, THRESHOLD_REG, threshold);      //设置门限
    kldaq_write_reg(g_fsmc_dev, CHANGERATE_REG, change_rate);   //设置变化率
    kldaq_write_reg(g_fsmc_dev, FILTER_MODE_REG, kldaq_read_reg(g_fsmc_dev, FILTER_MODE_REG) | (mode << 3)); //读取并设置滤波器 bit4-bit6

    return 0;
}

int daq_fsmc_channel_pd_cfg_set(uint8_t channel, uint16_t threshold, uint16_t change_rate)
{
    return kldaq_set_channel_pd_cfg(g_fsmc_dev, channel, threshold, change_rate);
}

uint32_t daq_fsmc_pd_ns_get(uint8_t channel)
{
    return kldaq_get_pd_ns(g_fsmc_dev, channel);
}
int daq_fsmc_pd_seconds_get(uint32_t *timestamp)
{
    return kldaq_get_pd_curve_time(g_fsmc_dev, timestamp);
}

int daq_fsmc_pd_flag_reset(uint16_t channels)
{
    return kldaq_reset_pd_flag(g_fsmc_dev, channels);
}

int daq_fsmc_pd_work_mode_set(uint8_t mode)
{
    return kldaq_set_pd_work_mode(g_fsmc_dev, mode);
}

int daq_fsmc_fpga_version_get(uint16_t *fpga_version)
{
    return kldaq_read_fpga_version(g_fsmc_dev, fpga_version);
}

int daq_fsmc_fpga_time_set(uint32_t timestamp)
{
    return kldaq_set_fpga_time(g_fsmc_dev, timestamp);
}

void daq_fsmc_fpga_power_reset(void)
{
    kldaq_power(g_fsmc_dev, KLFPGA_POWER_DOWN); // add by Xiewei, power off 1 seconds, and then power on
    xtimer_sleep(1);
    kldaq_power(g_fsmc_dev, KLFPGA_POWER_UP);
}

int daq_fsmc_prpd_sample_status_set(uint8_t status)
{
    uint16_t value = 0;
    value = kldaq_read_reg(g_fsmc_dev, PRPD_STATUS_REG);
    value &= ~(1 << 1);
    kldaq_write_reg(g_fsmc_dev, PRPD_STATUS_REG, value | ((!!status) << 1));

    return 0;
}

void daq_fsmc_fpga_work_status_check(kldaq_dev_t* dev)
{
    if (kldaq_is_microcode_work_ok(dev)) {
        LOG_INFO("Get FSMC FPGA work status as: OK!");
    }
    else {
        LOG_WARN("Get FSMC FPGA work status as: NOK!!");
    }
}

void check_spi_fpga_work_status(kldaq_dev_t* dev)
{
    if (kldaq_is_microcode_work_ok(dev)) {
        LOG_INFO("Get SPI FPGA work status as: OK!");
    }
    else {
        LOG_WARN("Get SPI FPGA work status as: NOK!!");
    }
}

void daq_init(void)
{
   kldaq_init(g_fsmc_dev);
   daq_fsmc_fpga_work_status_check(g_fsmc_dev);

   kldaq_init(g_spi_dev);
   check_spi_fpga_work_status(g_spi_dev);
}

void set_sample_curve_ops(kldaq_dev_t *dev)
{
    g_fsmc_dev = dev;
}

void set_sample_curve_with_mock_ops(void)
{
    g_fsmc_dev = (kldaq_dev_t*)&fpga_fsmc_mock_with_ps_cnf_dev;
    g_spi_dev = (kldaq_dev_t*)&fpga_spi_mock_with_ps_cnf_dev;
    kldaq_init(g_fsmc_dev);
}

void set_sample_curve_with_real_ops(void)
{
    g_fsmc_dev = (kldaq_dev_t*)&fpga_fsmc_dev_without_img_download;
    g_spi_dev = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;
}
/*****************************************************************************
 * SPI FPGA OPS for over voltage
 *****************************************************************************/
int daq_spi_sample_done_check(uint8_t channel)
{
    return kldaq_is_sample_done(g_spi_dev, channel);
}

uint32_t daq_spi_sample_len_get(uint8_t channel)
{
    return kldaq_get_length(g_spi_dev, channel);
}

int daq_spi_fpga_version_read(uint16_t *fpga_version)
{
    return kldaq_read_fpga_version(g_spi_dev, fpga_version);
}

int daq_spi_sample_data_read(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
    return kldaq_read_data(g_spi_dev, channel, data, addr, byte_len);
}

int daq_spi_curve_max_get(uint8_t channel)
{
    return kldaq_get_max(g_spi_dev, channel);
}

int daq_spi_channel_threshold_set(uint8_t channel, uint16_t threshold)
{
    return kldaq_set_threshold(g_spi_dev, channel, threshold);
}

int daq_spi_channel_changerate_set(uint8_t channel, uint16_t changerate)
{
    return kldaq_set_changerate(g_spi_dev, channel, changerate);
}

int daq_spi_ov_flag_reset(uint8_t channel)
{
    kldaq_trigger_one_sample(g_spi_dev, channel);
    return 0;
}
