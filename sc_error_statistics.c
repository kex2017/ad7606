#include "sc_error_statistics.h"

#include "string.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _packet_statistics_t{
   size_t packet_snd_num;
   size_t packet_rcv_num;
   size_t packet_rcv_cs_err_num;
   size_t common_errnum[10];
   size_t voltage_errnum[4];
   size_t pd_errnum[4];
}PACKET_STATISTICS;

static PACKET_STATISTICS pkt_statistics = { 0 };


void statistics_rcv_num(void)
{
    pkt_statistics.packet_rcv_num++;
}

void statistics_snd_num(void)
{
    pkt_statistics.packet_snd_num++;
}

void statistics_rcv_cs_err_num(void)
{
    pkt_statistics.packet_rcv_cs_err_num++;
}

void statistics_hf_current_jf_error_num(uint8_t channel, uint16_t num)
{
    pkt_statistics.common_errnum[channel] += num;
}

void statistics_dielectric_loss_error_num(uint8_t channel, uint16_t num)
{
    pkt_statistics.common_errnum[channel] += num;
}

void statistics_over_voltage_error_num(uint8_t channel, uint16_t num)
{
    pkt_statistics.voltage_errnum[channel] += num;
}

void statistics_partial_discharge_error_num(uint8_t channel, uint16_t num)
{
    pkt_statistics.pd_errnum[channel] += num;
}

uint32_t calc_all_type_error_num(void)
{
    uint32_t all = 0;
    for(uint8_t i  = 0; i < 10; i++)
    {
        all += pkt_statistics.common_errnum[i];
    }

    for(uint8_t i  = 0; i < 4; i++)
    {
        all += pkt_statistics.voltage_errnum[i];
    }

    for(uint8_t i  = 0; i < 4; i++)
    {
        all += pkt_statistics.pd_errnum[i];
    }

    return all;
}

uint32_t calc_hf_current_jf_error_num(void)
{
    uint32_t all = 0;
    for(uint8_t i  = 0; i < 4; i++)
    {
        all += pkt_statistics.common_errnum[i];
    }

    return all;
}

uint32_t calc_dielectric_loss_error_num(void)
{
    uint32_t all = 0;
    for(uint8_t i  = 4; i < 10; i++)
    {
        all += pkt_statistics.common_errnum[i];
    }

    return all;
}

uint32_t calc_over_voltage_error_num(void)
{
    uint32_t all = 0;
    for(uint8_t i  = 0; i < 4; i++)
    {
        all += pkt_statistics.voltage_errnum[i];
    }

    return all;
}

uint32_t calc_partial_discharge_error_num(void)
{
    uint32_t all = 0;
    for(uint8_t i  = 0; i < 4; i++)
    {
        all += pkt_statistics.pd_errnum[i];
    }

    return all;
}


void display_all_error_info(void)
{
    printf("all error info:\n"
    "    hf_current_jf error retransmission num:        %d\n"
    "    dielectric_loss error retransmission num:      %d\n"
    "    over_voltage error retransmission num:         %d\n"
    "    partial_discharge error retransmission num:    %d\n"
    "    all error retransmission num:                  %d\n"
    "    packet receive num:                            %d\n"
    "    packet send    num:                            %d\n"
    "    packet cs error num:                           %d\n"
     ,(int)calc_hf_current_jf_error_num(),
     (int)calc_dielectric_loss_error_num(),
     (int)calc_over_voltage_error_num(),
     (int)calc_partial_discharge_error_num(),
     (int)calc_all_type_error_num(),
     pkt_statistics.packet_rcv_num,
     pkt_statistics.packet_snd_num,
     pkt_statistics.packet_rcv_cs_err_num);
}

void display_channel_error_info(uint8_t channel)
{
    if(channel <= 3)
    {
        printf("channel error info:\n"
        "    hf_current_jf channel:%d    error count:%d\n"
        "    over_voltage  channel:%d    error count:%d\n",channel, (int)pkt_statistics.common_errnum[channel], channel, (int)pkt_statistics.voltage_errnum[channel]);
        return;
    }
    if(4 <= channel && channel <= 9)
    {
        printf("channel error info:\n"
            "    dielectric_loss channel:%d    error count:%d\n", channel, (int)pkt_statistics.common_errnum[channel]);
        return;
    }

    printf("Input error channel:%d\r\n",(int)channel);
}

void clear_all_num(void)
{
    memset(&pkt_statistics, 0x0, sizeof(pkt_statistics));
    printf("clear successful\r\n");
}

static void usage_help(void)
{
    printf("\nOptions:\n"
    "    -h, --help\n"
    "        print this help message\n"
    "    -a  --all\n"
    "        display all error info\n"
    "    -s <channel>, --select=<channel>\n"
    "        display one channel error info\n"
    "    -c --clear\n"
    "        clear all error info ,resumed\n"
    );
}

int statistics_command(int argc, char **argv)
{
    int opt = 0;
    uint8_t channel = 0;
    static const struct option long_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { "all", no_argument, NULL, 'a' },
        { "select", required_argument, NULL, 's' },
        { "clear", no_argument, NULL, 'c' },
        {NULL,0,NULL,0},
    };

    if(argc < 2)
    {
        usage_help();
        return 1;
    }

    while((opt = getopt_long(argc, argv, "has:c", long_opts, NULL)) != -1)
    {
        switch (opt) {
        case 'h':
            usage_help();
            break;
        case 'a':
            display_all_error_info();
            break;
        case 's':
            channel = (uint8_t)atoi(optarg);
            display_channel_error_info(channel);
            break;
        case 'c':
            clear_all_num();
            break;
        default:
            if(!optarg )
            {
                printf("optarg is NULL!\r\n");
                break;
            }
            printf("unkown command %s!\r\n",optarg);
            break;
        }
    }
    optind = 1;
    return 0;
}
