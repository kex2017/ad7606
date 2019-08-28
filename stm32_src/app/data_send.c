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
#include "periph/rtt.h"
#include "type_alias.h"
#include "data_send.h"
#include "daq.h"
#include "gps_sync.h"
#include "over_current.h"

#define PF_CHAN_0 0
#define PF_CHAN_1 1

#define HF_CHAN_0 2
#define HF_CHAN_1 3
void send_high_current_cycle_data(uint8_t send_type)
{
    uint8_t data[256] = { 0 };
    uint16_t len = 0;
    uint32_t hf_cur[MAX_HF_OVER_CURRENT_CHANNEL_COUNT] = { 0 };

    for (uint8_t phase = 0; phase < 3; phase++) {
        for (uint8_t channel = 0; channel < MAX_HF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            hf_cur[channel] = get_hf_max(phase, channel);
            LOG_INFO("hf cur phase %d channel[%d]: max data is %ld", phase, channel, hf_cur[channel]);
        }
        len = current_cycle_data_encode(data, DEVICEOK, send_type, MAX_HF_OVER_CURRENT_CHANNEL_COUNT, HF_CHAN_0 + phase*4,
                                        hf_cur[0], HF_CHAN_1 + phase * 4, hf_cur[1], rtt_get_counter());
        msg_send_pack(data, len);
    }
}

void send_pf_current_cycle_data(uint8_t send_type)
{
    uint16_t len = 0;
    uint8_t data[256] = { 0 };
    float pf_cur[MAX_PF_OVER_CURRENT_CHANNEL_COUNT] = { 0 };

    for (uint8_t phase = 0; phase < 3; phase++) {
        for (uint8_t channel = 0; channel < MAX_HF_OVER_CURRENT_CHANNEL_COUNT; channel++) {
            pf_cur[channel] = get_pf_rms(phase, channel);
            LOG_INFO("pf cur phase %d channel[%d] rms data is : %f", phase, channel, pf_cur[channel]);
        }
        len = current_cycle_data_encode(data, DEVICEOK, send_type, MAX_PF_OVER_CURRENT_CHANNEL_COUNT, PF_CHAN_0 + phase*4,
                                        (uint32_t)pf_cur[0], PF_CHAN_1 + phase*4, (uint32_t)pf_cur[1], rtt_get_counter());
        msg_send_pack(data, len);
    }
}

uint8_t server_call_data_flag = 0;

void set_server_call_flag(void)
{
    server_call_data_flag = 1;
}

void clear_server_call_flag(void)
{
    server_call_data_flag = 0;
}

uint8_t get_server_call_flag(void)
{
    return server_call_data_flag;
}

uint8_t get_send_type(void)
{
    if (get_server_call_flag()) {
          clear_server_call_flag();
          return 1;//召唤模式
      }
      else {
          return 0;//普通模式
      }
}


void send_over_current_curve(over_current_data_t* over_current_data, uint8_t channel, uint8_t send_type)
{
    uint16_t len = 0;
    uint8_t data[MAX_FRAME_LEN] = {0};
    uint8_t pk_data[PACKET_DATA_LEN] = {0};
    uint16_t pkg_num = 0;
    uint8_t left_data_len = 0;
    uint32_t timestamp = over_current_data->timestamp;

    if(HF_TYPE == over_current_data->data_type)
        channel = over_current_data->phase * 4 + channel + 2;//hf A:2,3 B:6,7 C:10,11
    else
        channel = over_current_data->phase * 4 + channel - 2;//pf A:0,1 B:4,5 C:6,7

    pkg_num = over_current_data->curve_len / PACKET_DATA_LEN;
    if ((left_data_len = (over_current_data->curve_len % PACKET_DATA_LEN))) {
        pkg_num += 1;
    }

    for (uint16_t i = 0; i < pkg_num; i++) {
        memset(pk_data, 0, PACKET_DATA_LEN);
        if (left_data_len && (i == pkg_num - 1)) {
            memcpy(pk_data, ((uint8_t*)over_current_data->curve_data) + i * PACKET_DATA_LEN, left_data_len);
            len = current_mutation_data_encode(data, DEVICEOK, send_type, timestamp, over_current_data->one_sec_clk_cnt, over_current_data->ns_cnt, channel, pkg_num, i,
                                                    pk_data, left_data_len);
        }
        else {
            memcpy(pk_data, ((uint8_t*)over_current_data->curve_data) + i * PACKET_DATA_LEN, PACKET_DATA_LEN);
            len = current_mutation_data_encode(data, DEVICEOK, send_type, timestamp, over_current_data->one_sec_clk_cnt, over_current_data->ns_cnt, channel, pkg_num, i,
                                                    pk_data, PACKET_DATA_LEN);
        }
        LOG_INFO("send over current curve data channel %d pkg num is %d cur pkg num is %d", channel, pkg_num, i);
        msg_send_pack(data, len);
        delay_ms(600);
    }
}

void send_dip_angle_data(void)
{
    uint16_t len = 0;
    uint8_t data[MAX_FRAME_LEN] = {0};
    GY25* p_gy25 = get_gy25_dip_angle();

    len = dip_angle_cycle_data_encode(data, DEVICEOK, rtt_get_counter(), p_gy25->course_angle, p_gy25->pitch_angle, p_gy25->roll_angle);
    msg_send_pack(data, len);
}


static msg_t send_task_rcv_queue[8];

static void upload_period_data(uint8_t send_type)
{
    LOG_INFO("send cycle data");
    send_high_current_cycle_data(send_type);
    send_pf_current_cycle_data(send_type);
    send_dip_angle_data();

}

void *data_send_serv(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_init_queue(send_task_rcv_queue, 8);
    uint8_t send_type = 0;
    while (1) {
        msg_receive(&msg);
        send_type = msg.content.value;
        upload_period_data(send_type);
        delay_ms(100);
    }
}

static char data_send_thread_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t data_send_serv_init(void)
{
    kernel_pid_t _pid = thread_create(data_send_thread_stack, sizeof(data_send_thread_stack),
                                      DATA_SEND_PRIORITY,
                                      THREAD_CREATE_STACKTEST, data_send_serv, NULL, "data send serv");
    period_data_hook(_pid);
    request_data_hook(_pid);
    return _pid;
}

