/*
 * sc_data_acquisition.c
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "log.h"
#include "x_delay.h"
#include "periph/rtt.h"
#include "board.h"

#include "data_acquisition.h"
#include "partial_discharge.h"
#include "acquisition_trigger.h"
#include "frame_handler.h"
#include "curve_harmonics.h"
#include "data_storage.h"
#include "daq.h"
#include "heart_beat.h"

#define PARTIAL_DISCHARGE 1
#define COMMON_CURVE 0
static uint8_t g_operation_type = COMMON_CURVE;

void sample_usage(void)
{
    printf("usage: sample commands:\r\n");
    printf("\t -h, --help\r\n");
    printf("\t\t print this help message\r\n");
    printf("\t -s <seconds>, --start=<seconds>\r\n");
    printf("\t\t trigger sample n seconds later\r\n");
    printf("\t -S, --Signal\r\n");
    printf("\t\t trigger Signal to FPGA right now\r\n");
    printf("\t -c <channel>, --curve=<channel>\r\n");
    printf("\t\t read sample curve data\r\n");
    printf("\t -l <channel>, --len=<channel>\r\n");
    printf("\t\t read sample curve data length\r\n");
    printf("\t -p <channel>, --pf=<channel>\r\n");
    printf("\t\t read sample power frequency current data\r\n");
    printf("\t -q <channel>, --query=<channel>\r\n");
    printf("\t\t query partial discharge event info\r\n");
    printf("\t -n, --notify\r\n");
    printf("\t\t send partial discharge event notify\r\n");
    printf("\t -t, --type=<0/1>\r\n");
    printf("\t\t show current timestamp and set default operation curve\r\n");
    printf("\t -T, --Threshold=<threshold>\r\n");
    printf("\t\t set all partial discharge channel threshold\r\n");
    printf("\t -C, --ChangeRate=<changeRate>\r\n");
    printf("\t\t set all partial discharge channel change rate\r\n");
    printf("\t -f, --flag\r\n");
    printf("\t\t reset all partial discharge channel flag\r\n");
    printf("\t -i, --irq=<enable/disable>\r\n");
    printf("\t\t switch partial discharge IRQ flag: 0:disable; !0:enable\r\n");
    printf("\t -R, --Read=<register>\r\n");
	printf("\t\t read FPGA register value\r\n");
	printf("\t -W, --Write=<register>\r\n");
	printf("\t\t write FPGA register value, always with option -v=<value>\r\n");
	printf("\t -v, --value=<value>\r\n");
	printf("\t\t write FPGA register value, always with option -W=<register>\r\n");
	printf("\t -g, --gain=<gain>\r\n");
	printf("\t\t set channel:0~3 JF gain\r\n");
	printf("\t -H, --Heartbeat\r\n");
	printf("\t\t Try to send mock heartbeat to master.\r\n");
	printf("\t -F, --FFT=<channel>\r\n");
	printf("\t\t Show last sampled channel's harmonics information.\r\n");
    printf("\t -w, --wdt=<flag>\r\n");
    printf("\t\t Update watchdog flag: 0:disable, 1:enable.\r\n");
}

void test_data_acquisition_set_trigger_sammple_info(uint16_t seconds)
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

void test_data_acquisition_read_curve_data(uint8_t channel)
{
    uint8_t data[250] = {0};
    uint16_t total_pkt_count = 0;
    uint16_t cur_pkt_num = 0; //TODO: add cur_pkt_num, sn in shell command line
    uint16_t count_index = 0;
    uint32_t timestamp = get_channel_sample_timestamp(channel); //read latest curve data

    if (COMMON_CURVE == g_operation_type) {
    	data_acquisition_read_curve_data(channel, timestamp, 250, cur_pkt_num, (void*)data, &total_pkt_count);
    }
    else {//partial discharge original data
    	partial_discharge_get_curve_data(channel, count_index, 250, cur_pkt_num, data);
    }
}

void test_data_acquisition_get_curve_data_len(uint8_t channel)
{
    uint32_t timestamp = 1; //read latest curve data
    uint16_t count_index = 0;
    uint32_t len = 0;

    if (COMMON_CURVE == g_operation_type) {
    	data_acquisition_get_curve_data_len(channel, timestamp, &len);
    }
    else {
    	partial_discharge_get_curve_data_len(channel, count_index, &len);
    }
}

void test_data_acquisition_read_pf_current(uint8_t channel)
{
    float pf_value[4] = {0};
    uint32_t timestamp = rtt_get_counter();

    data_acquisition_read_pf_current(channel, timestamp, (void*)pf_value);
}

void test_partial_discharge_query_event_info(uint8_t channel)
{
	uint8_t i;
	//4: means max event count in single channel, which defined in data_storage.h
	partial_discharge_event_info_t event_info[MAX_PARTIAL_DISCHARGE_EVENT_COUNT] = {0};

	partial_discharge_query_event_info(channel, (void*)event_info);

	printf("Partial discharge event info for channel:%d.\r\n", channel);
	for (i = 0; i < MAX_PARTIAL_DISCHARGE_EVENT_COUNT; i++) {
		printf("Event info: happened_flag:%d, sn:%d, len:%ld, seconds:%ld, nanoseconds:%ld, amplitude:%f.\r\n", !!event_info[i].happened_flag,
				event_info[i].sn, event_info[i].data_len, event_info[i].second, event_info[i].nanosecond, event_info[i].amplitude);;
	}
}

void test_partial_discharge_event_happen_notify(void)
{
	partial_discharge_event_happen_notify();
	printf("Send partial discharge event notify message.\r\n");
}

void test_partial_discharge_set_pd_channel_cfg(uint16_t threshold)
{
	partial_discharge_set_channel_cfg_by_index(0, threshold, threshold);
	partial_discharge_set_channel_cfg_by_index(1, threshold, threshold);
	partial_discharge_set_channel_cfg_by_index(2, threshold, threshold);
	printf("Set partial discharge all channel cfg as threshold:%d, change_rate:%d.\r\n", threshold, threshold);
}

void test_clear_pd_flag(void)
{
	daq_fsmc_pd_flag_reset(0x1c00);
	printf("Reset partial discharge all channel flag.\r\n");
}

void test_set_default_operation_curve(uint8_t type) {

	if (type) {
		g_operation_type = COMMON_CURVE;
	} else {
		g_operation_type = PARTIAL_DISCHARGE;
	}
	LOG_WARN("Show timestamp and set operation type as:%s.", type ? "partial discharge" : "common curve");
}

void test_set_irq_flag(uint8_t enable)
{
	if (!!enable) {
		enable_partial_discharge_irq();
		printf("Enable partial discharge IRQ.\r\n");
	}
	else {
		disable_partial_discharge_irq();
		printf("Disable partial discharge IRQ.\r\n");
	}
}

void test_set_jf_gain(int gain)
{
	int i = 0;

	printf("Set channel:0~3 JF gain as %d(DB)\r\n", gain);
	for (i = 0; i < 4; i++) {
		hal_JF_gain_out(i, gain);
		daq_fsmc_write_reg(14, (uint16_t)gain);
		set_channel_gain_cfg(i, gain);
	}
}

void test_data_acquisition_read_jf_current(void)
{
	data_acquisition_read_jf_current(0);
}

void test_send_heartbeat(void)
{
   printf("Try to send mock heartbeat to master.\r\n");
   heart_beat_handler();
}

float data[58] = {0};
int sample_command(int argc, char **argv)
{
    int opt = 0;
    int i = 0;
    uint16_t seconds = 0;
    uint8_t channel = 0;
    uint8_t type = 0;
    uint16_t pd_flag= 0, threshold = 2047, changeRate = 4095;
    uint8_t enable = 0;
    int readReg = -1, writeReg = -1, value = -1, gain = -1;
    static const struct option long_opts[] = {
            { "help", no_argument, NULL, 'h' },
            { "start", required_argument, NULL, 's' },
            { "Signal", required_argument, NULL, 'S' },
            { "curve", required_argument, NULL, 'c' },
            { "len", required_argument, NULL, 'l' },
            { "pf", required_argument, NULL, 'p' },
            { "query", required_argument, NULL, 'q'},
            { "notify", no_argument, NULL, 'n'},
            { "type", required_argument, NULL, 't' },
            { "Threshold", required_argument, NULL, 'T' },
            { "ChangeRate", required_argument, NULL, 'C' },
            { "flag", no_argument, NULL, 'f' },
            { "irq", no_argument, NULL, 'i' },
            { "gain", required_argument, NULL, 'g'},
            { "jufang", no_argument, NULL, 'j'},
            { "Heartbeat", no_argument, NULL, 'H'},
            { "FFT", required_argument, NULL, 'F'},
            { "wdt", required_argument, NULL, 'w'},
            {NULL,0,NULL,0},
        };

    if (argc < 2 || strlen(argv[1]) < 2) {
        sample_usage();
        return 1;
    }

    while((opt = getopt_long(argc, argv, "hs:Sc:l:p:q:nt::T:C:fi:g:jHF:w:", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'h':
                sample_usage();
                break;
            case 's':
                seconds = (uint16_t)atoi(optarg);
                test_data_acquisition_set_trigger_sammple_info(seconds);
                break;
            case 'S':
            	trigger_signal_by_hand();
            	break;
            case 'c':
                channel = (uint8_t)atoi(optarg);
                test_data_acquisition_read_curve_data(channel);
                break;
            case 'l':
                channel = (uint8_t)atoi(optarg);
                test_data_acquisition_get_curve_data_len(channel);
                break;
            case 'p':
                channel = (uint8_t)atoi(optarg);
                test_data_acquisition_read_pf_current(channel);
                break;
            case 'q':
            	channel = (uint8_t)atoi(optarg);
            	test_partial_discharge_query_event_info(channel);
            	break;
            case 'n':
            	test_clear_pd_flag();
            	test_partial_discharge_event_happen_notify();
            	break;
            case 't':
            	type = (uint8_t)atoi(optarg);
            	test_set_default_operation_curve(type);
                break;
            case 'T':
            	pd_flag = 1;
            	threshold = (uint16_t)atoi(optarg);
            	continue;
            case 'C':
            	pd_flag = 1;
            	changeRate = (uint16_t)atoi(optarg);
            	continue;
            case 'f':
            	test_clear_pd_flag();
            	break;
            case 'i':
            	enable = (uint8_t)atoi(optarg);
            	test_clear_pd_flag();
            	test_set_irq_flag(enable);
            	break;
            case 'g':
            	gain = atoi(optarg);
            	test_set_jf_gain(gain);
            	break;
            case 'j':
            	test_data_acquisition_read_jf_current();
            	break;
            case 'H':
               test_send_heartbeat();
               break;
            case 'F':
               channel = atoi(optarg);
               show_last_sampled_harmonics_info(channel);
               break;
            case 'w':
               value = atoi(optarg);
               update_wdt_flag(!!value);
               break;
            default:
                /* once option without valid value, opt will be set as '?' and optarg is NULL,
                 * so will need to handle here to avoid segmentation fault.*/
                if (!optarg) {
                    printf("optarg is NULL\r\n");
                    break;
                }
                printf("unknown command %s!\r\n", optarg);
                break;
        }
    }

    if (writeReg != -1 && value != -1) {
    	printf("Try to write FPGA register:%d with value:%d.\r\n", writeReg, value);
    	daq_fsmc_write_reg((uint16_t)writeReg, (uint16_t)value);
    }

	if (readReg != -1) {
		printf("Try to read FPGA register:%d with value:0x%04x.\r\n", readReg, daq_fsmc_read_reg((uint16_t)readReg));
	}

	if (pd_flag) {
		for (i = 0; i < MAX_PARTIAL_DISCHARGE_AND_PRPD_CHANNEL_COUNT; i++) {
			partial_discharge_set_channel_cfg_by_index(i, threshold, changeRate);
		}
		printf("Set all PD channel threshold:%d, changeRate:%d.\r\n", threshold, changeRate);
	}


    optind = 1;

    return 0;

}
