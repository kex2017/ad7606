/*
 * data_processor.c
 *
 *  Created on: Apr 16, 2019
 *      Author: xuke
 */
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "fault_location_threads.h"

#include "thread.h"
#include "periph/rtt.h"
#include "internal_ad_sample.h"
#include "periph/stm32f10x_std_periph.h"
#include "timex.h"
#include "xtimer.h"
#include "log.h"
#include "heart_beat.h"
#include "x_delay.h"
#include "env_cfg.h"

__IO uint16_t sample_buf[CHANNEL_COUNT * SAMPLE_COUNT];

uint16_t pf_adc1_channel_list[PF_CHANNEL_COUNT] = {10, 11};


static kernel_pid_t to_receive_pid;

static msg_t rcv_queue[8];

static int mutation_msg_is_done;

static float rms_data[PF_CHANNEL_COUNT] = {0};

static MUTATION_DATA mutation_data;

static int mutation_msg_is_done;
static int periodic_msg_is_done;

PF_DATA pf_data;

msg_t sample_done_msg;
msg_t send_mutation_msg;


int pf_set_threshold_changerate(uint8_t channel, uint16_t threshold,uint16_t changerate)
{
    pf_data.pf_threshold_chanagerate[channel].pf_threshold = threshold;
    pf_data.pf_threshold_chanagerate[channel].pf_chanagerate = changerate;
   return 0;
}

void pf_set_over_current_cal_k_b(uint8_t channel, pf_cal_k_b_t pf_cal_k_b)
{
    pf_data.pf_threshold_chanagerate[channel].pf_cal_k_b = pf_cal_k_b;
}
uint16_t pf_get_threshold(uint8_t channel) 
{
    return pf_data.pf_threshold_chanagerate[channel].pf_threshold;
}
uint16_t pf_get_changerate(uint8_t channel)
{
    return pf_data.pf_threshold_chanagerate[channel].pf_chanagerate;
}
pf_cal_k_b_t get_pf_over_current_cal_k_b(uint8_t channel)
{
    return pf_data.pf_threshold_chanagerate[channel].pf_cal_k_b;
}
void set_default_pf_threshold_rate(void)
{
    uint16_t default_threshold = 2047;
    uint16_t default_changerate = 4095;

    for (int i = 0; i < CHANNEL_COUNT; i++) {
        pf_set_threshold_changerate(i,default_threshold,default_changerate);
        LOG_INFO("Set pf over current threshold and changerate for Channel %d: 0x%04X ,0x%04X", i, default_threshold,
                 default_changerate);
    }
}

void init_msg_send_is_done(void)
{
   mutation_msg_is_done = 1;
   periodic_msg_is_done =1;
}

void send_mutation_msg_is_done(void)
{
   mutation_msg_is_done = 1;
}

void calc_rms(RAW_DATA *raw_data, float *rms_data)
{
    uint32_t quadratic_sum[CHANNEL_COUNT] = {0};

    for (int channel = 0; channel < CHANNEL_COUNT; channel++)
    {
        for (int i = 0; i < SAMPLE_COUNT; i++)
        {
            quadratic_sum[channel] += (uint32_t)((raw_data->data[CHANNEL_COUNT * i + channel] - 2048) * (raw_data->data[CHANNEL_COUNT * i + channel] - 2048));
        }
        rms_data[channel] = sqrt((double)quadratic_sum[channel] / SAMPLE_COUNT);
    }
}

int detect_mutation(float *rms_data,RAW_DATA *raw_data)
{
    int i = 0;
    for(i=0;i<CHANNEL_COUNT;i++)
    {
        if (rms_data[i] > pf_data.pf_threshold_chanagerate[i].pf_threshold)
        {
            return 1;
        }
        if((uint32_t)fabs((double)raw_data->data[0]-(double)raw_data->data[31]) >= pf_data.pf_threshold_chanagerate[i].pf_chanagerate)
        {
            return 1;
        }
    }

    return 0;
}

static kernel_pid_t self_pid;
static kernel_pid_t data_recv_pid;
RAW_DATA irq_packet[5];
msg_t irq_msg[5];
int irq_packet_i;

void pf_data_recv_hook(kernel_pid_t pid)
{
    data_recv_pid = pid;
}
static void pf_sample_buff_cb(void)
{
    if (irq_packet_i == 7)
    {
        irq_packet_i = 0;
    }
    // printf("====================/r/n");
    memset((void *)irq_packet[irq_packet_i].data, 0, CHANNEL_COUNT * SAMPLE_COUNT * 2);
    memcpy((void *)irq_packet[irq_packet_i].data, (void *)sample_buf, CHANNEL_COUNT * SAMPLE_COUNT * 2);

    irq_msg[irq_packet_i].content.ptr = (void *)&irq_packet[irq_packet_i];

    int ret = 0;
    ret = msg_try_send(&(irq_msg[irq_packet_i]), self_pid);

    if (ret == -1)
    {
        printf("data collection send msg error \r\n");
    }

    // printf("add sample to raw_data finish\r\n");
    // printf("irq_packet_i = %d\r\n", irq_packet_i);
    irq_packet_i++;
}

static void pf_sample_init(void)
{
    adc1_dma1_tim4_sample_init(pf_adc1_channel_list, CHANNEL_COUNT, SAMPLE_COUNT, SAMPLE_DURATION_MS, (uint16_t *)sample_buf);
    set_adc1_dma1_tim4_sample_done_cb(pf_sample_buff_cb);
}

void do_receive_pid_hook(kernel_pid_t pid)
{
    to_receive_pid = pid;
}


int do_periodic_task;
int general_call_task;
msg_t send_periodic_msg;
static PERIODIC_DATA periodic_data  ;


void clear_periodic_task(void)
{
   do_periodic_task = 0;
}

void init_task(void)
{
   clear_periodic_task();
}
void *internal_ad_sample_serv(void *arg)
{
    (void)arg;
    msg_t msg;
    RAW_DATA *raw_data;
    int periodic_task_i = 0;

    init_task();
    init_msg_send_is_done();
    set_default_pf_threshold_rate();
    msg_init_queue(rcv_queue, 8);
    pf_sample_init();
    msg_receive(&msg);

    raw_data = (RAW_DATA *)(msg.content.ptr);
    while (1)
    {
        msg_receive(&msg);
        raw_data = (RAW_DATA *)(msg.content.ptr);
        calc_rms(raw_data, rms_data);
        // printf("raw_data[0] = %f,raw_data[1] = %f\r\n",rms_data[0],rms_data[1]);

        if (detect_mutation(rms_data,raw_data))
        {
            if (!mutation_msg_is_done)
            {
                printf("mutation data not send done\r\n");
                mutation_data.rms_data[0] = rms_data[0];
                mutation_data.rms_data[1] = rms_data[1];
                memcpy((void *)(&(mutation_data.wd.data[CHANNEL_COUNT * SAMPLE_COUNT])), (void *)sample_buf, CHANNEL_COUNT * SAMPLE_COUNT * 2);
                continue;
            }
            mutation_data.rms_data[0] = rms_data[0];
            mutation_data.rms_data[1] = rms_data[1];
            memcpy((void *)(&(mutation_data.wd.data[CHANNEL_COUNT * SAMPLE_COUNT])), (void *)sample_buf, CHANNEL_COUNT * SAMPLE_COUNT * 2);
            for(int i = 0;i < SAMPLE_COUNT;i++)
            {
                mutation_data.channel1[i] = raw_data->data[CHANNEL_COUNT * i + 0];
                mutation_data.channel2[i] = raw_data->data[CHANNEL_COUNT * i + 1];
            }
            send_mutation_msg.type = MUTATION_DATA_TYPE;
            send_mutation_msg.content.ptr = (void *)(&(mutation_data));
            msg_send(&(send_mutation_msg), data_recv_pid);

            periodic_task_i = 0;
            do_periodic_task = 0;
        }
        else
        {
            if (general_call_task)
            {
                ;
            }
            if (do_periodic_task)
            {

                if (periodic_task_i >= SAMPLE_COUNT)
                {
                    printf("periodic_task");
                    periodic_task_i = 0;
                    send_periodic_msg.type = PERIODIC_DATA_TYPE;
                    send_periodic_msg.content.ptr = (void *)(&periodic_data);
                    msg_send(&send_periodic_msg, data_recv_pid);
                    do_periodic_task = 0;
                    continue;
                }

                for (int i = 0; i < SAMPLE_COUNT; i++)
                {
                    periodic_data.channel1[periodic_task_i] = raw_data->data[CHANNEL_COUNT * i + 0];
                    periodic_data.channel2[periodic_task_i] = raw_data->data[CHANNEL_COUNT * i + 1];
                    periodic_task_i++;
                }
                periodic_data.rms_data[0] = rms_data[0];
                periodic_data.rms_data[1] = rms_data[1];

            }
        }
    }
}

void set_data_collection_receiver(kernel_pid_t pid)
{
    self_pid = pid;
}

static char vc_temp_bat_sample_thread_stack[THREAD_STACKSIZE_MAIN ];

kernel_pid_t internal_ad_sample_serv_init(void)
{
    kernel_pid_t _pid = thread_create(vc_temp_bat_sample_thread_stack, sizeof(vc_temp_bat_sample_thread_stack),
                                      VC_TEMP_BAT_SAMPLE_SERV_PRIORITY, THREAD_CREATE_STACKTEST, internal_ad_sample_serv, NULL,
                                      "Power frequency sample serv");
    set_data_collection_receiver(_pid);
    return _pid;
}





void pray_periodic_task(void)
{
   do_periodic_task = 1;
}

