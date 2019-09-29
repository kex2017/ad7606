#include <errno.h>

#include "log.h"
#include "mutex.h"
#include "xtimer.h"

#include "daq.h"
#include "kldaq.h"
#include "kldaq_fpga_spi.h"
#include "x_delay.h"

fpga_cs_t g_cur_fpga_cs;

kldaq_dev_t* g_spi_dev = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;

gpio_t fpga_cs_pin[] = {SPI1_A_CS, SPI1_B_CS, SPI1_C_CS};

void check_spi_fpga_work_status(kldaq_dev_t* dev)
{
    if (kldaq_is_microcode_work_ok(dev)) {
        printf("Get SPI FPGA work status as: OK!\r\n");
    }
    else {
        printf("Get SPI FPGA work status as: NOK!!\r\n");
    }
}

void daq_spi_cs_pin_init(void)
{
    gpio_init(SPI1_A_CS, GPIO_OUT);
    gpio_init(SPI1_B_CS, GPIO_OUT);
    gpio_init(SPI1_C_CS, GPIO_OUT);

    gpio_set(SPI1_A_CS);
    gpio_set(SPI1_B_CS);
    gpio_set(SPI1_C_CS);
}

void daq_init(void)
{
    kldaq_init(g_spi_dev);
    daq_spi_cs_pin_init();
//    check_spi_fpga_work_status(g_spi_dev);
}

void set_sample_curve_ops(kldaq_dev_t *dev)
{
    g_spi_dev = dev;
    daq_init();
}


void change_spi_cs_pin(fpga_cs_t cs_no)
{
    ((fpga_spi_dev_t *)g_spi_dev)->cs_pin = fpga_cs_pin[cs_no];
    g_cur_fpga_cs = cs_no;
    delay_ms(20);
//    LOG_INFO("change cs pin to %s\r\n", (cs_no==0)?"A":(cs_no==1)?"B":"C");
}

fpga_cs_t get_cur_fpga_cs(void)
{
    return g_cur_fpga_cs;
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
    uint16_t chan_status = 0;

    if (channel < 2) {
        chan_status = daq_spi_read_reg(channel, CHA_STA_REG);
        return chan_status & (1 << (1));
    }
    chan_status = daq_spi_read_reg(channel, LTC_OCR_REG);
    return chan_status & (1 << (channel-2));
}

uint16_t daq_spi_get_data_len(uint8_t channel)
{
    if(channel < 2)
        return daq_spi_read_reg(channel, DAT_LEN_L);
    return daq_spi_read_reg(channel, LTC_LENGTH_REG);
}

int daq_spi_get_dat_max(uint8_t channel)
{
    return daq_spi_read_reg(channel, DAT_MAX_REG);
}

int daq_spi_get_dat_avr(uint8_t channel)
{
    return daq_spi_read_reg(channel, DAT_AVR_REG);
}

uint32_t daq_spi_one_sec_clk_cnt(uint8_t chan_no)
{
    uint32_t clk_cnt = 0;

    if(chan_no == HF_CHANNEL_1_NUM){
        clk_cnt = daq_spi_read_reg(chan_no, CHA0_ONE_SECOND_CNT_H) << 16 | daq_spi_read_reg(chan_no, CHA0_ONE_SECOND_CNT_L);
    }
    else if(chan_no == HF_CHANNEL_2_NUM){
        clk_cnt = daq_spi_read_reg(chan_no, CHA1_ONE_SECOND_CNT_H) << 16 | daq_spi_read_reg(chan_no, CHA1_ONE_SECOND_CNT_L);
    }
    else if(chan_no == PF_CHANNEL_1_NUM){
        clk_cnt = daq_spi_read_reg(chan_no, CHA2_ONE_SECOND_CNT_H) << 16 | daq_spi_read_reg(chan_no, CHA2_ONE_SECOND_CNT_L);
    }
    else if(chan_no == PF_CHANNEL_2_NUM){
        clk_cnt = daq_spi_read_reg(chan_no, CHA3_ONE_SECOND_CNT_H) << 16 | daq_spi_read_reg(chan_no, CHA3_ONE_SECOND_CNT_L);
    }

    return clk_cnt;
}

uint32_t daq_spi_chan_cnt_since_plus(uint8_t chan_no)
{
    uint32_t chan_cnt = 0;

    if (chan_no == HF_CHANNEL_1_NUM){
        chan_cnt = daq_spi_read_reg(chan_no, CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H) << 16 | daq_spi_read_reg(chan_no, CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L);
    }
    else if(chan_no == HF_CHANNEL_2_NUM){
        chan_cnt = daq_spi_read_reg(chan_no, CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H) << 16 | daq_spi_read_reg(chan_no, CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L);
    }
    else if(chan_no == PF_CHANNEL_1_NUM){
        chan_cnt = daq_spi_read_reg(chan_no, LTC_GP_PLS_CNTA_H) << 16 | daq_spi_read_reg(chan_no, LTC_GP_PLS_CNTA_L);
    }
    else if(chan_no == PF_CHANNEL_2_NUM){
        chan_cnt = daq_spi_read_reg(chan_no, LTC_GP_PLS_CNTB_H) << 16 | daq_spi_read_reg(chan_no, LTC_GP_PLS_CNTB_L);

    }

    return chan_cnt;
}

uint32_t daq_spi_chan_event_utc(uint8_t chan_no)
{
    uint32_t fpga_utc = 0;

    if (chan_no == HF_CHANNEL_1_NUM) {
        fpga_utc = daq_spi_read_reg(chan_no, CHA0_EVENT_UTC_TIME_H) << 16 | daq_spi_read_reg(chan_no, CHA0_EVENT_UTC_TIME_L);
    }
    else if (chan_no == HF_CHANNEL_2_NUM) {
        fpga_utc = daq_spi_read_reg(chan_no, CHA1_EVENT_UTC_TIME_H) << 16 | daq_spi_read_reg(chan_no, CHA1_EVENT_UTC_TIME_L);
    }
    else if (chan_no == PF_CHANNEL_1_NUM) {
        fpga_utc = daq_spi_read_reg(chan_no, CHA2_EVENT_UTC_TIME_H) << 16 | daq_spi_read_reg(chan_no, CHA2_EVENT_UTC_TIME_L);
    }
    else if (chan_no == PF_CHANNEL_2_NUM) {
        fpga_utc = daq_spi_read_reg(chan_no, CHA3_EVENT_UTC_TIME_H) << 16 | daq_spi_read_reg(chan_no, CHA3_EVENT_UTC_TIME_L);
    }

    return fpga_utc;
}

void daq_spi_chan_set_fpga_utc(uint32_t utc_time)
{
    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin(phase);
        daq_spi_write_reg(0, SET_UTC_TIME_H, (utc_time >> 16) & 0xFFFF);
        daq_spi_write_reg(0, SET_UTC_TIME_L, utc_time & 0xFFFF);
    }
    change_spi_cs_pin(cur_fpga_cs);
}

void daq_spi_read_test_reg(void)
{
    uint16_t test_reg_fix = 5555;
    uint16_t read_value = 0;

    read_value = daq_spi_read_reg(0, FPGA_READ_TEST_REG);
    if(read_value == test_reg_fix){
        LOG_INFO("succeed!!!  read value is 0x%04x\r\n", read_value);
    }
    else{
        LOG_ERROR("failed!!!  read value is 0x%04x\r\n", read_value);
    }
}

/*write ops*/
void daq_spi_trigger_sample(uint8_t channel)
{
    daq_spi_write_reg(channel, TRIGGER_REG, 1);
}

int daq_spi_clear_data_done_flag(uint8_t channel)
{
    switch(channel){
    case HF_CHANNEL_1_NUM:
        daq_spi_write_reg(channel, CHA_RUN_REG, 1 << 4);
        break;
    case HF_CHANNEL_2_NUM:
        daq_spi_write_reg(channel, CHA_RUN_REG, 1 << 5);
        break;
    case PF_CHANNEL_1_NUM:
        daq_spi_write_reg(channel, LTC_FINSH_CHB_REG_H, 1 << 0);
        break;
    case PF_CHANNEL_2_NUM:
        daq_spi_write_reg(channel, LTC_FINSH_CHB_REG_H, 1 << 1);
        break;
    default:
        break;
    }
    return 0;
}

int daq_spi_set_hf_threshold(uint8_t channel, uint16_t threshold)
{
    daq_spi_write_reg(channel, DAT_THR_REG, threshold);
    return 0;
}

int daq_spi_set_hf_change_rate(uint8_t channel, uint16_t changerate)
{
    daq_spi_write_reg(channel, CHANGE_RATE_THR, changerate);
    return 0;
}

int daq_spi_set_pf_threshold(uint8_t channel, uint64_t  threshold)
{
    uint16_t th_l = threshold & 0xFFFF;
    uint16_t th_m = (threshold >> 16) & 0xFFFF;
    uint16_t th_h = (threshold >> 32) & 0xFFFF;

    if(channel == PF_CHANNEL_1_NUM){
        daq_spi_write_reg(channel, LTC_OTH_CHA_REG_L, th_l);
        daq_spi_write_reg(channel, LTC_OTH_CHA_REG_M, th_m);
        daq_spi_write_reg(channel, LTC_OTH_CHA_REG_H, th_h);
    }
    else if(channel == PF_CHANNEL_2_NUM){
        daq_spi_write_reg(channel, LTC_OTH_CHB_REG_L, th_l);
        daq_spi_write_reg(channel, LTC_OTH_CHB_REG_M, th_m);
        daq_spi_write_reg(channel, LTC_OTH_CHB_REG_H, th_h);
    }
    return 0;
}

uint64_t daq_spi_get_pf_sum_data(uint8_t channel)
{
    uint16_t fpga_sum_data[3] = {0};
    uint64_t sum = 0;

    if(channel == PF_CHANNEL_1_NUM){
        fpga_sum_data[0] = daq_spi_read_reg(channel, LTC_GP_SQRT_CNTA_H);
        fpga_sum_data[1] = daq_spi_read_reg(channel, LTC_GP_SQRT_CNTA_M);
        fpga_sum_data[2] = daq_spi_read_reg(channel, LTC_GP_SQRT_CNTA_L);

        sum = fpga_sum_data[1] << 16 | fpga_sum_data[2];
        sum = sum & 0xFFFFFFFF;
        sum |= (uint64_t)fpga_sum_data[0] << 32;

        return sum;
    }
    else if(channel == PF_CHANNEL_2_NUM){
        fpga_sum_data[0] = daq_spi_read_reg(channel, LTC_GP_SQRT_CNTB_H);
        fpga_sum_data[1] = daq_spi_read_reg(channel, LTC_GP_SQRT_CNTB_M);
        fpga_sum_data[2] = daq_spi_read_reg(channel, LTC_GP_SQRT_CNTB_L);
        sum = fpga_sum_data[1] << 16 | fpga_sum_data[2];
        sum = sum & 0xFFFFFFFF;
        sum |= (uint64_t)fpga_sum_data[0] << 32;

        return sum;
    }
    return 0;
}

