#include "sc_daq.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "daq.h"
#include "log.h"
#include "periph/rtt.h"
#include "acquisition_trigger.h"
#include "cable_ground_threads.h"
#include "upgrade_from_sd_card.h"

void send_sample_data_msg(void)
{
    msg_t msg;
    static msg_read_info_t g_sample_info = {0};

    msg.type = MSG_READ_COMMON;
    g_sample_info.channels = 0x3FF; //0-9 channels
    g_sample_info.timestamp = rtt_get_counter();
    msg.content.ptr = &g_sample_info;
    msg_send_int(&msg, data_acquisition_service_pid);
}

void daq_usage_help(void)
{
   printf("\nCommands:\n"
   "    --------------daq fsmc fpga ops---------------\n"
   "    mock  :set daq interface to mock mode\n"
   "    real  :set daq interface to real mode\n"
   "    mode <mode> :set pd work mode(0:auto 1:manual)\n"
   "    single <channel> :sample one Channel\n"
   "    multi :sample 0-5 channel\n"
   "    fisdone <channel>: check if sample done\n"
   "    fgetlen <channel>: Get Channel data length\n"
   "    frddata <channel>: Read data from Channel\n"
   "    setcfg <channel> <threshold> <changerate>:set Channel threshold and change_rate\n"
   "    query <channel>: query if over threshold on Channel\n"
   "    rstpd  :reset pd flag on Channel\n"
   "    getns <channel>: Get Channel pd ns \n"
   "    settm <timestamp>:set fpga timestamp\n"
   "    gettm :get pd curve timestamp\n"
   "    readv :read fpga version\n"
   "    status <status>:set prpd sample status\n"
   "    rdreg <reg> :read fpga reg value\n"
   "    wrreg <reg> <reg_value>:write fpga reg with reg value\n"
   "    --------------daq  spi fpga ops---------------\n"
   "    sisdone <channel>: check if sample done\n"
   "    sgetlen <channel>: Get Channel data length\n"
   "    srddata <channel>: Read data from Channel\n"
   "    sgetmax <channel>: Get max value on Channel\n"
   "    ssetth <channel> <threshold>:set Channel threshold\n"
   "    ssetch <channel> <changerate>:set Channel changerate\n"
   "    rstov <channel> :reset ov flag on Channel\n"
   "    savesd <flag> :set save curve data to sd(1:save 0:not)\n"
   );
}

static uint8_t g_sample_buf[512] __attribute__((section(".big_data")));

int daq_command(int argc, char **argv)
{
    if (argc < 2) {
        daq_usage_help();
        return 1;
    }
    if (strncmp(argv[1], "mock", 5) == 0) {
        set_sample_curve_with_mock_ops();
        printf("set fsmc mock mode successful! now daq data from mock\r\n");
    }
    else if (strncmp(argv[1], "real", 5) == 0) {
        set_sample_curve_with_real_ops();
        printf("set fsmc real mode successful! now daq data from fpga\r\n");
    }
    else if (strncmp(argv[1], "rdreg", 6) == 0) {
        uint16_t reg = (uint16_t)strtol(argv[2], NULL, 10);

        uint16_t reg_value = daq_fsmc_read_reg(reg);
        printf("read fsmc fpga reg(%d):%d successful!\r\n", reg, reg_value);
    }
    else if (strncmp(argv[1], "wrreg", 6) == 0) {
        uint16_t reg = (uint16_t)strtol(argv[2], NULL, 10);
        uint16_t reg_value = (uint16_t)strtol(argv[3], NULL, 10);

        daq_fsmc_write_reg(reg, reg_value);
        printf("write fsmc fpga reg(%d):%d successful!\r\n", reg, reg_value);
    }
    else if (strncmp(argv[1], "single", 7) == 0) {
        uint8_t channel = 0;
        channel = (uint8_t)strtol(argv[2], NULL, 10);

        daq_fsmc_single_chan_sample(channel);
        send_sample_data_msg();
        printf("Trigger fsmc fpga one shot on Channel %d successful!\r\n", channel);
    }
    else if (strncmp(argv[1], "multi", 6) == 0) {
        uint8_t channels[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        daq_fsmc_multi_chans_sample(channels, sizeof(channels));
        send_sample_data_msg();
        printf("Trigger fsmc fpga sample on 0-9 Channels successful!\r\n");
    }
    else if (strncmp(argv[1], "fisdone", 8) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);

        uint8_t done_flag = daq_fsmc_sample_done_check(channel);
        if (done_flag > 0) {
            printf("fsmc fpga sample Channel %d already done!\r\n", channel);
        }
        else {
            printf("fsmc fpga sample Channel %d not done yet!\r\n", channel);
        }
    }
    else if (strncmp(argv[1], "fgetlen", 8) == 0) {
        uint8_t channel = 0;
        uint32_t len = 0;
        channel = (uint8_t)strtol(argv[2], NULL, 10);

        len = daq_fsmc_sample_len_get(channel);
        printf("Get fsmc fpga Channel %d sample data length is:%"PRIu32"\r\n", channel, len);
    }
    else if (strncmp(argv[1], "frddata", 8) == 0) {
        uint8_t channel = 0, len = 128;
        channel = (uint8_t)strtol(argv[2], NULL, 10);

        daq_fsmc_sample_data_read(channel, g_sample_buf, 0, len);
        printf("Read data from fsmc fpga Channel %d successful!\r\n", channel);
        for (int i = 0; i < 128; i++) {
            printf("0x%04x ", g_sample_buf[i * 2 + 1] << 8 | g_sample_buf[i * 2]);
            if((i+1)%8 == 0)
                puts("\r\n");
        }
    }
    else if (strncmp(argv[1], "fgetmax", 8) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);

        uint16_t max_value = daq_fsmc_curve_max_value_get(channel);
        printf("Get max value on fsmc fpga Channel %d:%d successful!\r\n", channel, max_value);
    }
    else if (strncmp(argv[1], "setcfg", 9) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t threshold = (uint16_t)strtol(argv[3], NULL, 10);
        uint16_t change_rate = (uint16_t)strtol(argv[4], NULL, 10);

        daq_fsmc_channel_pd_cfg_set(channel, threshold, change_rate);
        printf("set fsmc fpga pd Channel %d threshold as:%d change_rate:%d successful!\r\n", channel, threshold, change_rate);
    }
    else if (strncmp(argv[1], "query", 6) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);

        int over_flag = daq_fsmc_over_threshold_check(channel);
        if (over_flag > 0)
            printf("over threshold on fsmc fpga Channel:%d!\r\n", channel);
        else
            printf("not over threshold on fsmc fpga Channel:%d!\r\n", channel);
    }
    else if (strncmp(argv[1], "setfre", 7) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t start_fre = (uint16_t)strtol(argv[3], NULL, 10);
        uint16_t end_fre = (uint16_t)strtol(argv[4], NULL, 10);
        uint16_t threshold = (uint16_t)strtol(argv[5], NULL, 10);
        uint16_t change_rate = (uint16_t)strtol(argv[6], NULL, 10);
        uint8_t mode = (uint8_t)strtol(argv[7], NULL, 10);

        daq_fsmc_channel_cfg_set(channel, start_fre, end_fre, threshold, change_rate, mode);
        printf("set fsmc fpga Channel %d start_fre as:%d end_fre as:%d threshold as:%d change_rate as:%d mode as:%d successful!\r\n", channel, start_fre, end_fre, threshold, change_rate, mode);
    }
    else if (strncmp(argv[1], "getns", 6) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);

        uint32_t ns = daq_fsmc_pd_ns_get(channel);
        printf("Get pd ns on fsmc fpga Channel %d:%ld successful!\r\n", channel, ns);
    }
    else if (strncmp(argv[1], "gettm", 6) == 0) {
        uint32_t timestamp = 0;

        daq_fsmc_pd_seconds_get(&timestamp);
        printf("get fsmc fpga PD timestamp as :%ld successful!\r\n", timestamp);
    }
    else if (strncmp(argv[1], "rstpd", 6) == 0) {
        daq_fsmc_pd_flag_reset(0x1C00);
        printf("reset all pd flag on fsmc fpga Channel successful!\r\n");
    }
    else if (strncmp(argv[1], "mode", 5) == 0) {
        uint8_t mode = (uint8_t)atol(argv[2]);

        daq_fsmc_pd_work_mode_set(mode);
        printf("set fsmc fpga pd work mode as :%d successful!\r\n", mode);
    }
    else if (strncmp(argv[1], "readv", 6) == 0) {
        uint16_t fpga_version[32] = { 0 };

        daq_fsmc_fpga_version_get(fpga_version);
        printf("read fsmc fpga version:%s\r\n", (char*)fpga_version);
    }
    else if (strncmp(argv[1], "settm", 6) == 0) {
        uint32_t timestamp = (uint32_t)atol(argv[2]);

        daq_fsmc_fpga_time_set(timestamp);
        printf("set fsmc fpga timestamp as :%ld successful!\r\n", timestamp);
    }
    else if (strncmp(argv[1], "status", 7) == 0) {
        uint8_t status = (uint8_t)atol(argv[2]);

        daq_fsmc_prpd_sample_status_set(status);
        printf("set fsmc fpga prpd sample status as:%d successful!\r\n", status);
    }
    else if (strncmp(argv[1], "sisdone", 8) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
        int done_flag = daq_spi_sample_done_check(channel);

        if (done_flag > 0) {
            printf("spi fpga sample Channel %d already done!\r\n", channel);
        }
        else {
            printf("spi fpga sample Channel %d not done yet!\r\n", channel);
        }
    }
    else if (strncmp(argv[1], "sgetlen", 8) == 0) {
        uint8_t channel = 0;
        uint32_t len = 0;
        channel = (uint8_t)strtol(argv[2], NULL, 10);
        len = daq_spi_sample_len_get(channel);
        printf("Get length on spi fpga Channel %d:%"PRIu32" successful!\r\n", channel, len);
    }
    else if (strncmp(argv[1], "srddata", 8) == 0) {
        uint8_t channel = 0, len = 128;
        channel = (uint8_t)strtol(argv[2], NULL, 10);
        daq_spi_sample_data_read(channel, g_sample_buf, 0, len);
        printf("Read data from spi fpga Channel %d successful!\r\n", channel);
        for(int i = 0;i < 128;i++)
        {
            printf("%02x ", g_sample_buf[i]);
        }
    }
    else if (strncmp(argv[1], "sgetmax", 8) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t max_value = daq_spi_curve_max_get(channel);
        printf("Get max value on spi fpga Channel %d:%d successful!\r\n", channel, max_value);
    }
    else if (strncmp(argv[1], "ssetth", 7) == 0) {
        uint8_t channel = 0;
        uint16_t threshold = 0x3ff0;
        channel = (uint8_t)strtol(argv[2], NULL, 10);
        daq_spi_channel_threshold_set(channel, threshold);
        printf("Set threshold for spi fpga ov Channel %d successful!\r\n", channel);
    }
    else if (strncmp(argv[1], "ssetch", 7) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
        uint16_t changerate = (uint16_t)strtol(argv[3], NULL, 10);
        daq_spi_channel_changerate_set(channel, changerate);
        printf("set changerate on spi fpga ov Channel %d:%d successful!\r\n", channel, changerate);
    }
    else if (strncmp(argv[1], "rstov", 7) == 0) {
        uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
        daq_spi_ov_flag_reset(channel);
        printf("reset ov flag on spi fpga Channel %d successful!\r\n", channel);
    }
    else if (strncmp(argv[1], "savesd", 6) == 0) {
        uint8_t flag = (uint8_t)strtol(argv[2], NULL, 10);
        if (flag) {
            set_save_curve_data_to_sd_flag();
            printf("start save curve data to sdcard\r\n");
        }
        else {
            clear_save_curve_data_to_sd_flag();
            printf("stop save curve data to sdcard\r\n");
        }
    }
   else {
      daq_usage_help();
   }

   return 0;
}
