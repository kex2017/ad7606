#include <stdint.h>
#include "period_data.h"
#include "fault_location_threads.h"
#include "frame_encode.h"
#include "frame_handler.h"
#include "thread.h"
#include "periph/rtt.h"
#include "env_cfg.h"
#include "x_delay.h"
#include "type_alias.h"
#include "over_current.h"
#include "periph/rtt.h"
#include "data_send.h"
#include "internal_ad_sample.h"

void send_high_current_cycle_data(void)
{
    uint8_t data[256] = { 0 };
    uint16_t len = 0;

    cal_k_b_t k_b[MAX_OVER_CURRENT_CHANNEL_COUNT] = { 0 };
    uint32_t hf_cur[MAX_OVER_CURRENT_CHANNEL_COUNT] = { 0 };

    for (uint8_t channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++) {
        k_b[channel] = get_over_current_cal_k_b(channel);
        hf_cur[channel] = k_b[channel].k * get_over_current_max(channel) + k_b[channel].b;
    }
    len = high_current_cycle_data_encode(data, DEVICEOK, hf_cur[0], hf_cur[1], rtt_get_counter());
    msg_send_pack(data, len);
}

void send_over_current_curve(over_current_data_t* over_current_data, uint8_t channel)
{
    uint16_t len = 0;
    uint8_t data[MAX_FRAME_LEN] = {0};
    uint8_t pk_data[PACKET_DATA_LEN] = {0};
    uint16_t pkg_num = 0;
    uint8_t left_data_len = 0;

    pkg_num = over_current_data->curve_len / PACKET_DATA_LEN;
    if ((left_data_len = (over_current_data->curve_len % PACKET_DATA_LEN))) {
        pkg_num += 1;
    }
    for (uint16_t i = 0; i < pkg_num; i++) {
        memset(pk_data, 0, PACKET_DATA_LEN);
        if (left_data_len && (i == pkg_num - 1)) {
            memcpy(pk_data, over_current_data->curve_data + i * PACKET_DATA_LEN, left_data_len);
            len = high_current_mutation_data_encode(data, DEVICEOK, over_current_data->ns_cnt, channel, pkg_num, i,
                                                    pk_data, left_data_len);
        }
        else {
            memcpy(pk_data, over_current_data->curve_data + i * PACKET_DATA_LEN, PACKET_DATA_LEN);
            len = high_current_mutation_data_encode(data, DEVICEOK, over_current_data->ns_cnt, channel, pkg_num, i,
                                                    pk_data, PACKET_DATA_LEN);
        }
        msg_send_pack(data, len);
        delay_ms(100);
    }
}

void send_mutation_data(MUTATION_DATA* md)
{
   uint8_t data[1024] = {0};
   uint16_t length = 0;

   LOG_INFO("start send mutatuin time");

   length = current_mutation_data_encode(data,0x01,rtt_get_counter(),CHANNEL_1,1,0,(uint8_t*)md->channel1,SAMPLE_COUNT);
   msg_send_pack(data,length);

   length = current_mutation_data_encode(data,0x01,rtt_get_counter(),CHANNEL_2,1,0,(uint8_t*)md->channel1,SAMPLE_COUNT);
   msg_send_pack(data,length);
   LOG_INFO("send mutatuin data done");
}

void send_general_call_data(GENERAL_CALL_DATA* gd)
{
   uint8_t data[1024] = {0};
   uint16_t length = 0;

   LOG_INFO("start send general call data time");

   length = current_cycle_data_encode(data,0x01,(uint32_t)gd->rms_data[0],(uint32_t)gd->rms_data[1],rtt_get_counter());
   msg_send_pack(data,length);

   LOG_INFO("send general call data done");

}

void send_periodic_data(PERIODIC_DATA* pd)
{
   uint8_t data[1024] = {0};
   uint16_t length = 0;

   LOG_INFO("start send periodic time");

   length = current_cycle_data_encode(data,0x01,(uint32_t)pd->rms_data[0],(uint32_t)pd->rms_data[1],rtt_get_counter());
   msg_send_pack(data,length);

   LOG_INFO("send periodic data done");

}

static msg_t send_task_rcv_queue[8];

static void upload_period_data(void)
{
    send_high_current_cycle_data();
}

void *data_send_serv(void *arg)
{
    (void)arg;
    msg_t msg;
    MUTATION_DATA* md;
    PERIODIC_DATA* pd;
    GENERAL_CALL_DATA *gd;
    msg_init_queue(send_task_rcv_queue, 8);
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case PERIOD_DATA_TYPE:
            upload_period_data();
            break;
        case PF_PERIOD_DATA_TYPE:
            pd = (PERIODIC_DATA*)(msg.content.ptr);
            send_periodic_data(pd);
            break;
        case PF_CURVE_TYPE:
            md = (MUTATION_DATA*)(msg.content.ptr);
            send_mutation_data(md);
            send_mutation_msg_is_done();
            break;
        case GENERAL_CALL_DATA_TYPE:
            gd = (GENERAL_CALL_DATA*)(msg.content.ptr);
            send_general_call_data(gd);
            break;
        default:
            break;
        }
        delay_ms(100);
    }
}

static char data_send_thread_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t data_send_serv_init(void)
{
    kernel_pid_t _pid = thread_create(data_send_thread_stack, sizeof(data_send_thread_stack),
                                      DATA_SEND_PRIORITY,
                                      THREAD_CREATE_STACKTEST, data_send_serv, NULL, "period data serv");
    period_data_hook(_pid);
    pf_data_recv_hook(_pid);
    request_data_hook(_pid);
    return _pid;
}

