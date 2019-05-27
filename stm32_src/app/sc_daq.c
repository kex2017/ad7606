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


void daq_usage_help(void)
{
   printf("\nCommands:\n"
   "    th <channel> <threshold>:set pd tw threshold\n"
   "    pd <channel> <threshold>:set pd tw change threshold\n"
   "    wrreg <channel> <reg> <value>:write reg\n"
   "    rdreg <channel> <reg>:read reg\n"
   "    rddata <channel> <len> : read data from fpga\n"
   "    isdone <channel>:check if sample done\n"
   "    len <channel> :read data length\n"
   "    test <channel>: test start-isdone-len-rddata\n"
   "    clear <channel> <chip_no>: clear channel cfg chip done flag\n"
   "    peak <channel>: read peak\n"
   "    dcnum <channel>:read discharge num\n"
   "    clkcnt :read one sec clk cnt\n"
   "    ltcnum :read ltc num since plus\n"
   );
}

static uint8_t g_data_buf[4096];
int daq_command(int argc, char **argv)
{
    if (argc < 2) {
        daq_usage_help();
        return 1;
    }

   if (strncmp(argv[1], "wrreg", 6) == 0) {
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
        uint32_t  len = daq_spi_get_data_len(channel);
        LOG_INFO("read data length is :%ld!", len);
    }
    else if (strncmp(argv[1], "th", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  threshold = (uint16_t)strtol(argv[3], NULL, 10);
        daq_spi_set_threshold(channel, threshold);
        LOG_INFO("set pthreshold with channel %d threshold %d!", channel, threshold);
    }
    else if (strncmp(argv[1], "ch", 8) == 0) {
        uint8_t  channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t  changerate = (uint16_t)strtol(argv[3], NULL, 10);
        daq_spi_set_change_rate(channel, changerate);
        LOG_INFO("set  change rate with channel %d threshold %d!", channel, changerate);
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
        daq_spi_read_test_reg();
    }
   else {
      daq_usage_help();
   }

   return 0;
}

