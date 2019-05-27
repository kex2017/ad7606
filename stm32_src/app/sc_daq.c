#include "sc_daq.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "board.h"
#include "daq.h"
#include "log.h"
#include "periph/rtt.h"
#include "kldaq_fpga_spi.h"
#include "switch_value.h"
//#include "data_transfer.h"
#include "adc_gain_set.h"


void daq_usage_help(void)
{
   printf("\nCommands:\n"
   "    sw <sw_no>:read switch value\n"
   "    th <channel> <threshold>:set pd tw threshold\n"
   "    pd <channel> <threshold>:set pd tw change threshold\n"
   "    wrreg <channel> <reg> <value>:write reg\n"
   "    rdreg <channel> <reg>:read reg\n"
   "    rddata <channel> <len> : read data from fpga\n"
   "    start  : start sample\n"
   "    isdone <channel>:check if sample done\n"
   "    len <channel> :read data length\n"
   "    test <channel>: test start-isdone-len-rddata\n"
   "    cfgdata <channel> <chip_no> <data>:cfg chip data\n"
   "    cisdone <channel> <chip_no>: check if cfg chip done\n"
   "    clear <channel> <chip_no>: clear channel cfg chip done flag\n"
   "    peak <channel>: read peak\n"
   "    dcnum <channel>:read discharge num\n"
   "    clkcnt :read one sec clk cnt\n"
   "    ltcnum :read ltc num since plus\n"
   );
}

static uint8_t g_data_buf[2048];
int daq_command(int argc, char **argv)
{
    if (argc < 2) {
        daq_usage_help();
        return 1;
    }

   if (strncmp(argv[1], "gain", 5) == 0) {
        uint8_t chan_type = (uint8_t)strtol(argv[2], NULL, 10);
        uint8_t phase = (uint8_t)strtol(argv[3], NULL, 10);
        uint8_t gain_level = (uint8_t)strtol(argv[4], NULL, 10);
        set_gain_by_chan_type_and_phase(chan_type, phase, gain_level);
        LOG_INFO("set gain to type:%d , phase:%d, gain_level:%d ok!", chan_type, phase, gain_level);
    }

    else if (strncmp(argv[1], "sw", 5) == 0) {
        uint8_t sw_no = (uint8_t)strtol(argv[2], NULL, 10);
        uint8_t sw_val = read_sw(sw_no);
       LOG_INFO("read switch no.%d value :%d", sw_no, sw_val);
    }
    else if (strncmp(argv[1], "wrreg", 6) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  reg = (uint16_t)strtol(argv[3], NULL, 10);
        uint16_t  value = (uint16_t)strtol(argv[4], NULL, 10);
        daq_spi_write_reg(channel, reg, value);
        LOG_INFO("write reg with channel %d reg %d value %d!", channel, reg, value);
      }
    else if (strncmp(argv[1], "rdreg", 6) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  reg = (uint16_t)strtol(argv[3], NULL, 10);
        uint16_t value = daq_spi_read_reg(channel, reg);
        LOG_INFO("read reg with channel %d reg %d value %d!", channel, reg, value);
    }
    else if (strncmp(argv[1], "peak", 5) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  peak = daq_spi_get_dat_max(channel);
        LOG_INFO("read peak with channel:%d peak:%d!", channel, peak);
    }
    else if (strncmp(argv[1], "dcnum", 6) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  discharge_num = daq_spi_get_discharge_num(channel);
        LOG_INFO("read discharge num with channel:%d discharge num:%d!", channel, discharge_num);
    }
    else if (strncmp(argv[1], "clkcnt", 7) == 0) {
        uint32_t  sec_clk_cnt = daq_spi_one_sec_clk_cnt();
        LOG_INFO("read one sec clk cnt:%ld!", sec_clk_cnt);
    }
    else if (strncmp(argv[1], "ltcnum", 7) == 0) {
        uint32_t  ltc_num = daq_spi_ltc_num_since_plus();
        LOG_INFO("read ltc num since plus:%ld!", ltc_num);
    }
    else if (strncmp(argv[1], "start", 6) == 0) {
        daq_spi_start_sample();
        LOG_INFO("start sample");
    }
    else if (strncmp(argv[1], "isdone", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        int done_flag = daq_spi_sample_done_check(channel);

        if (done_flag > 0)
            LOG_INFO("fpga sample already done!");
        else
            LOG_INFO("fpga sample  not done yet!");
    }
    else if (strncmp(argv[1], "len", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint32_t  len = daq_spi_sample_len_get(channel);
        LOG_INFO("read data length is :%ld!", len);
    }
    else if (strncmp(argv[1], "th", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  threshold = (uint16_t)strtol(argv[3], NULL, 10);
        daq_spi_set_pd_tw_threshold(channel, threshold);
        LOG_INFO("set pd tw threshold with channel %d threshold %d!", channel, threshold);
    }
    else if (strncmp(argv[1], "pd", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  threshold = (uint16_t)strtol(argv[3], NULL, 10);
        daq_spi_set_pd_tw_change_threshold(channel, threshold);
        LOG_INFO("set pd tw change threshold with channel %d threshold %d!", channel, threshold);
    }
    else if (strncmp(argv[1], "cfgdata", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint8_t  chip_no = (uint8_t)strtol(argv[3], NULL, 10);
        uint16_t  data = (uint16_t)strtol(argv[4], NULL, 10);
        daq_spi_cfg_chip_data(channel, chip_no, data);
        LOG_INFO("cfg channel %d chip%d with data%d!", channel, chip_no, data);
    }
    else if (strncmp(argv[1], "cisdone", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint8_t  chip_no = (uint8_t)strtol(argv[3], NULL, 10);
        uint8_t done_flag = daq_spi_cfg_chip_is_done(channel, chip_no);
        if(done_flag){
            LOG_INFO("cfg channel %d chip %d done", channel, chip_no);
        }
        else{
            LOG_INFO("cfg not done yet!");
        }
    }
    else if (strncmp(argv[1], "clear", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint8_t  chip_no = (uint8_t)strtol(argv[3], NULL, 10);
        daq_spi_cfg_chip_clear_done_flag(channel, chip_no);
        LOG_INFO("clear channel %d chip %d done flag", channel, chip_no);
    }

    else if (strncmp(argv[1], "rddata", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t len = (uint16_t)strtol(argv[3], NULL, 10);
        daq_spi_sample_data_read(channel, g_data_buf, 0, len);
        LOG_INFO("Read %d data from fpga", len);
        for (int i = 0; i < len; i++)
            printf("%02x ", g_data_buf[i]);
    }
    else if (strncmp(argv[1], "test", 6) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint32_t data_len = 0;
        daq_spi_start_sample();
        if(daq_spi_sample_done_check(channel)){
            data_len = daq_spi_sample_len_get(channel);
            daq_spi_sample_data_read(channel, g_data_buf, 0, data_len);
            printf("read data len is %ld data as:\r\n", data_len);
            for(uint32_t i = 0; i < data_len; i++){
                printf("%02x ",g_data_buf[i]);
            }
            puts("\r\n");
        }
        LOG_INFO("sample not done yet!");
    }
   else {
      daq_usage_help();
   }

   return 0;
}

