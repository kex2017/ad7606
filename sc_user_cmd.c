#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "daq.h"
#include "log.h"
#include "board.h"
#include "heart_beat.h"
#include "periph/rtt.h"
#include "over_voltage.h"
#include "data_storage.h"
#include "data_acquisition.h"
#include "acquisition_trigger.h"
#include "partial_discharge.h"
#include "curve_harmonics.h"

void use_cmd_usage(void)
{
    printf("\nCommands:\n"
    "    sec <seconds>: sample 0-9 channel data after <seconds>s\n"
    "    gain <gain>:set high  frequency gain to channel(0-2) \n"
    "    hfcur :Get high  frequency current on channel(0-2)\n"
    "    pfcur :Get power frequency current on Channel(4-9)\n"
    "    ovirq <flag> :open/close ov irq(flag:0(close) 1(open)\n"
    "    pdirq <flag>:open/close pd irq(flag:0(close) 1(open)\n"
    "    ovtrg <channel> :set threshold&change_rate to 0 to trigger ov event\n"
    "    pdtrg <channel> :set threshold&change_rate to 0 to trigger pd event\n"
    "    ovent  :check is ov curve sample done\n"
    "    pdent  :check is pd curve sample done\n"
    "    ovinf :query over voltage event inf on channel(0-2)\n"
    "    pdinf :query partial discharge event info on channel(10-12)\n"
    "    heart : send heartbeat to java master\n"
    "    dishar <channel>:show last sampled harmonics info on channel(4-9)\n"
    );
}

#define MAX_FPGA_DATA_LEN (20480 + 4096) //24k
static uint8_t curve_buffer[MAX_FPGA_DATA_LEN] __attribute__((section(".big_data")));

static void test_data_acquisition_set_trigger_sammple_info(uint16_t seconds)
{
    uint8_t i;
    uint32_t timestamp = rtt_get_counter();

    printf("Now timestamp: %ld.\r\n", timestamp);
    timestamp += seconds;

    channel_cfg_t cfg[10];
    for (i = 0; i < 10; i++) {
        cfg[i] = get_channel_sample_cfg(i);
    }

    if (0 == data_acquisition_set_trigger_sammple_info(timestamp, 10, cfg)) {
        printf("Set trigger %d seconds later to sample all channels' curve data.\r\n", seconds);
    }
}

static void test_data_acquisition_read_pf_current(void)
{
    float pf_value[4] = {0};
    uint32_t timestamp = rtt_get_counter();

    data_acquisition_read_pf_current(0, timestamp, (void*)pf_value);
}
static void test_data_acquisition_read_jf_current(void)
{
    data_acquisition_read_jf_current(0);
}

static void test_send_heartbeat(void)
{
   printf("Try to send mock heartbeat to master.\r\n");
   heart_beat_handler();
}

static void test_partial_discharge_query_event_info(void)
{
    uint8_t i;
    //4: means max event count in single channel, which defined in data_storage.h
    partial_discharge_event_info_t event_info[3][MAX_PARTIAL_DISCHARGE_EVENT_COUNT] = {0};
    for(uint8_t channel = 0;channel < 3;channel++){
    partial_discharge_query_event_info(channel, event_info[channel]);

    printf("Partial discharge event info for channel:%d.\r\n", channel);
    for (i = 0; i < MAX_PARTIAL_DISCHARGE_EVENT_COUNT; i++) {
        printf("Event info: happened_flag:%d, sn:%d, len:%ld, seconds:%ld, nanoseconds:%ld, amplitude:%f.\r\n", !!event_info[channel][i].happened_flag,
                event_info[channel][i].sn, event_info[channel][i].data_len, event_info[channel][i].second, event_info[channel][i].nanosecond, event_info[channel][i].amplitude);;
    }
    }
}

static void test_set_jf_gain(int gain)
{
    int i = 0;

    printf("Set channel:0~3 JF gain as %d(DB)\r\n", gain);
    for (i = 0; i < 4; i++) {
        hal_JF_gain_out(i, gain);
        set_channel_gain_cfg(i, gain);
    }
}

int user_command(int argc, char **argv)
{
   uint8_t channels[4] = { 0 };
//   uint8_t channel=0;
   uint8_t i, count = 0;
   over_voltage_event_info_t event;
   uint32_t len = 0;
   uint16_t total_pkt_count;

   if (argc < 2) {
      use_cmd_usage();
      return 1;
   }

    if (strncmp(argv[1], "ovent", 6) == 0) {
        over_voltage_event_happen_notify();     //查询过压波形是否采集完成
    }
    else if (strncmp(argv[1], "pdent", 6) == 0) {
        partial_discharge_event_happen_notify();//查询局放波形是否采集完成
    }
    else if (strncmp(argv[1], "ovtrg", 6) == 0){
        uint8_t channel = 0;
        uint16_t threshold = 0x0, change_rate = 0;
        channel = (uint8_t)strtol(argv[2], NULL, 10);
        daq_spi_channel_threshold_set(channel, threshold);
        daq_spi_channel_changerate_set(channel, change_rate);
        printf("Set threshold&change_rate to 0 for Channel %d to trigger ov event!\r\n", channel);
    }
    else if (strncmp(argv[1], "pdtrg", 6) == 0){
        uint8_t channel = 0;
        uint16_t threshold = 0x0, change_rate = 0;
        channel = (uint8_t)strtol(argv[2], NULL, 10);
        daq_fsmc_channel_pd_cfg_set(channel, threshold, change_rate);
        printf("Set threshold&change_rate to 0 for Channel %d to trigger pd event!\r\n", channel);
    }
    else if (strncmp(argv[1], "ovirq", 8) == 0) {
        uint8_t enable = (uint8_t)strtol(argv[2], NULL, 10);
        if (!!enable) {
            enable_over_voltage_irq();
            printf("Enable over voltage IRQ.\r\n");
        }
        else {
            remove_ovirq_timer();
            disable_over_voltage_irq();
            printf("Disable over voltage IRQ.\r\n");
        }
    }
    else if (strncmp(argv[1], "pdirq", 8) == 0) {
        uint8_t enable = (uint8_t)strtol(argv[2], NULL, 10);
        if (!!enable) {
            enable_partial_discharge_irq();
            printf("Enable partial discharge IRQ.\r\n");
        }
        else {
            remove_pdirq_timer();
            disable_partial_discharge_irq();
            printf("Disable partial discharge IRQ.\r\n");
        }
    }
    else if (strncmp(argv[1], "sec", 4) == 0){
        uint16_t seconds = (uint16_t)strtol(argv[2], NULL, 10);
        test_data_acquisition_set_trigger_sammple_info(seconds);
    }
    else if (strncmp(argv[1], "pfcur", 6) == 0){
          test_data_acquisition_read_pf_current();
    }
    else if (strncmp(argv[1], "hfcur", 6) == 0){
          test_data_acquisition_read_jf_current();
    }
    else if (strncmp(argv[1], "heart", 7) == 0){
          test_send_heartbeat();
    }
    else if (strncmp(argv[1], "dishar", 7) == 0){
          uint8_t channel = (uint8_t)strtol(argv[2], NULL, 10);
          show_last_sampled_harmonics_info(channel);
    }
    else if (strncmp(argv[1], "pdinf", 6) == 0){
          test_partial_discharge_query_event_info();
    }
    else if (strncmp(argv[1], "gain", 5) == 0){
          uint8_t gain = (uint8_t)strtol(argv[2], NULL, 10);
          test_set_jf_gain(gain);
    }
    else if (strncmp(argv[1], "ovinf", 6) == 0) {
        over_voltage_get_event_happened_channels(channels, &count);
        printf("Query over voltage event count %d happened\r\n", count);
        for (i = 0; i < count; i++) {
            over_voltage_get_event_info(channels[i], &event);
            over_voltage_get_curve_data_len(channels[i], event.timestamp, &len);
            printf("Got channel[%d] over voltage details: len:%ld, time:%s, sn:%d, max_value:%d, k:%f\r\n", channels[i],
                   len, ctime((const time_t *)&(event.timestamp)), event.sn, event.max_value, event.k);
            if (len != 0) {
                over_voltage_get_curve_data(channels[i], event.timestamp, event.sn, 0, &total_pkt_count, curve_buffer,
                                            MAX_FPGA_DATA_LEN);
                LOG_DUMP((const char*)curve_buffer, 32);
            }
        }
    }
    else {
        use_cmd_usage();
    }

   return 0;

}
