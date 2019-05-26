#include <math.h>
#include <string.h>

#include "periph/rtt.h"
#include "log.h"

#include "daq.h"
#include "daq_curve_ops_mock.h"

/***************************************************************************************
 * version 2 for FPGA mock
 ***************************************************************************************/
#define PI 3.14159265
#define TEST_PRPS_DATA_LEN  ((20480 + 4096)/2)
#define TEST_PRPD_DATA_LEN  ((20480 + 4096)/2)

static uint16_t test_prps_data[TEST_PRPS_DATA_LEN]  __attribute__((section(".big_data")));
static uint16_t test_prpd_data[TEST_PRPD_DATA_LEN] __attribute__((section(".big_data")));
static int kldaq_fpga_mock_init(kldaq_dev_t *daq)
{
    (void)daq;
    LOG_INFO("kldaq fpga mock init");

    LOG_DEBUG("kldaq fpga mock init prps and prpd data");

    uint16_t *y = test_prps_data;
    for (int i = 0; i < TEST_PRPS_DATA_LEN; i++) {
        *y = (unsigned short)100 * (sin(i * PI / 200) + 1);
        y++;
    }

    y = test_prpd_data;
    for (int i = 0; i < TEST_PRPD_DATA_LEN / 4; i++) {
        *y++ = i % 200; //幅值
        *y++ = i % 200; //相位
        *y++ = 0; //周期
        *y++ = 0; //周期
    }

    return 0;
}

static int kldaq_fpga_mock_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel)
{
    (void)daq;
    (void)channel;
    return 0;
}

static int kldaq_fpga_mock_trigger_multi_channels_sample(kldaq_dev_t *daq, uint8_t *channels, size_t count)
{
    (void)daq;
    (void)channels;
    (void)count;
    return 0;
}

static int kldaq_fpga_mock_is_sample_done(kldaq_dev_t *daq, uint8_t channel)
{
    (void)daq;
    (void)channel;
    LOG_DEBUG("kldaq fpga mock is sample done");

    return 1;
}

static uint32_t kldaq_fpga_mock_get_length(kldaq_dev_t *daq, uint8_t channel)
{
    (void)daq;
    (void)channel;
    LOG_DEBUG("kldaq fpga mock get length");
    return (20480 + 4096);
}

static uint32_t kldaq_fpga_mock_get_pd_ns(kldaq_dev_t *daq, uint8_t channel)
{
    (void)daq;
    (void)channel;
    LOG_DEBUG("kldaq fpga mock get pd ns");
    return 20480;
}

static int kldaq_fpga_mock_read_data(kldaq_dev_t *daq, uint8_t channel, void *dest, uint32_t addr, size_t size)
{
    (void)daq;
    (void)addr;

    uint32_t i = 0;
    unsigned short *y = (unsigned short *)dest;
    unsigned int short_len = size / 2;

    LOG_DEBUG("kldaq fpga mock read data");
    if (channel < 13) { //PRPS and PD
        for (i = 0; i < short_len; i++) {
            y[i] = test_prps_data[i] * (channel+1);
        }
    }
    else { //PRPD(13-15)
        for (i = 0; i < TEST_PRPD_DATA_LEN; i++) {
            y[i] = test_prpd_data[i];
        }
    }

    return 0;
}

static uint16_t kldaq_fpga_mock_get_max(kldaq_dev_t *daq, uint8_t channel)
{
    (void)daq;

    LOG_DEBUG("kldaq fpga mock get max");
    return (channel + 1) * 100;
}

static int kldaq_fpga_mock_is_over_threshold(kldaq_dev_t *daq, uint8_t channel)
{
    (void)daq;
    (void)channel;

    LOG_DEBUG("kldaq fpga mock is over threshold");
    return 0;
}

static int kldaq_fpga_mock_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold)
{
    (void)daq;
    (void)channel;

    LOG_DEBUG("kldaq fpga mock set threshold");
    (void)threshold;

    return 0;
}

static int kldaq_fpga_mock_set_changerate(kldaq_dev_t *daq, uint8_t channel, uint16_t changerate)
{
    (void)daq;
    (void)channel;

    LOG_DEBUG("kldaq fpga mock set changerate");
    (void)changerate;
    return 0;
}

static int kldaq_fpga_mock_reset_pd_flag(kldaq_dev_t *daq, uint16_t channels)
{
    (void)daq;
    (void)channels;

    return 0;
}

static int kldaq_fpga_mock_set_channel_pd_cfg(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold, uint16_t change_rate)
{
    (void)daq;
    (void)channel;

    LOG_DEBUG("kldaq fpga mock set channel pd cfg");
    (void)threshold;
    (void)change_rate;

    return 0;
}

static int kldaq_fpga_mock_set_time(kldaq_dev_t *daq, uint32_t timestamp)
{
    (void)daq;
    (void)timestamp;
    LOG_DEBUG("kldaq fpga mock set time");
    return 0;
}

static int kldaq_fpga_mock_get_pd_curve_time(kldaq_dev_t *daq, uint32_t *timestamp)
{
    (void)daq;
    *timestamp = rtt_get_counter();
    LOG_DEBUG("kldaq fpga mock get pd curve time");
    return 0;
}

static int kldaq_fpga_mock_write_reg(kldaq_dev_t *daq, uint16_t reg, uint16_t value)
{
    (void)daq;
    (void)reg;
    (void)value;

    return 0;
}
static uint16_t kldaq_fpga_mock_read_reg(kldaq_dev_t *daq, uint16_t reg)
{
    (void)daq;
    (void)reg;
    LOG_DEBUG("kldaq fpga mock read reg");

    return 0;
}

static int kldaq_fpga_mock_read_version(kldaq_dev_t *daq, uint16_t *fpga_version)
{
    (void)daq;
    char *version = "klfpga_test_01";

    LOG_DEBUG("kldaq fpga mock read version");
    strncpy((char*)fpga_version, version, strlen(version));
    return 0;
}

static int kldaq_fpga_mock_set_pd_work_mode(kldaq_dev_t *daq, uint8_t mode)
{
    (void)daq;
    (void)mode;
    LOG_INFO("kldaq fpga mock set pd work mode as:%d", mode);
    return 0;
}

const kldaq_func_t kldaq_fpga_mock_driver = {
                .init                           = kldaq_fpga_mock_init,
                .trigger_single_sample          = kldaq_fpga_mock_trigger_one_sample,
                .trigger_multi_channels_sample  = kldaq_fpga_mock_trigger_multi_channels_sample,
                .is_sample_done                 = kldaq_fpga_mock_is_sample_done,
                .get_length                     = kldaq_fpga_mock_get_length,
                .get_pd_ns                      = kldaq_fpga_mock_get_pd_ns,
                .read_data                      = kldaq_fpga_mock_read_data,
                .get_max                        = kldaq_fpga_mock_get_max,
                .is_over_threshold              = kldaq_fpga_mock_is_over_threshold,
                .set_threshold                  = kldaq_fpga_mock_set_threshold,
                .set_changerate                 = kldaq_fpga_mock_set_changerate,
                .reset_pd_flag                  = kldaq_fpga_mock_reset_pd_flag,
                .set_channel_pd_cfg             = kldaq_fpga_mock_set_channel_pd_cfg,
                .set_fpga_time                  = kldaq_fpga_mock_set_time,
                .get_pd_curve_time              = kldaq_fpga_mock_get_pd_curve_time,
                .write_reg                      = kldaq_fpga_mock_write_reg,
                .read_reg                       = kldaq_fpga_mock_read_reg,
                .read_version                   = kldaq_fpga_mock_read_version,
                .set_pd_work_mode               = kldaq_fpga_mock_set_pd_work_mode,
};
fpga_mock_dev_t fpga_fsmc_mock_with_ps_cnf_dev = {
                .base   = {&kldaq_fpga_mock_driver, &kldaq_fpga_dummy_cnf_ops}
};

fpga_mock_dev_t fpga_spi_mock_with_ps_cnf_dev = {
                .base   = {&kldaq_fpga_mock_driver, &kldaq_fpga_dummy_cnf_ops}
};
