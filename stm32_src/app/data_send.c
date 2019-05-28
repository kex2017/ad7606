#include <stdint.h>
#include "period_data.h"

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

static void upload_period_data(void)
{
    send_high_current_cycle_data();
    //发送其他周期数据
}

#define PACKET_DATA_LEN 200
void send_over_current_curve(void)
{
    uint16_t len = 0;
    uint8_t data[256] = {0};
    uint8_t pk_data[PACKET_DATA_LEN] = {0};
    uint16_t pkg_num = 0;
    uint8_t left_data_len = 0;

    over_current_data_t *p_over_current_data = NULL;
    for(uint8_t channel = 0; channel < MAX_OVER_CURRENT_CHANNEL_COUNT; channel++){
        p_over_current_data = get_over_current_data(channel);
        if (p_over_current_data->happen_flag) {
            pkg_num = p_over_current_data->curve_len / PACKET_DATA_LEN;
            if ((left_data_len = (p_over_current_data->curve_len % PACKET_DATA_LEN))) {
                pkg_num += 1;
            }
            for(uint16_t i = 0; i < pkg_num; i++){
                memset(pk_data, 0, PACKET_DATA_LEN);
                if(left_data_len && (i == pkg_num-1)){
                    memcpy(pk_data, p_over_current_data->curve_data + i*PACKET_DATA_LEN, left_data_len);
                    len = high_current_mutation_data_encode(data, DEVICEOK, p_over_current_data->ns_cnt, channel, pkg_num, i, pk_data, left_data_len);
                }
                else{
                    memcpy(pk_data, p_over_current_data->curve_data + i*PACKET_DATA_LEN, PACKET_DATA_LEN);
                    len = high_current_mutation_data_encode(data, DEVICEOK, p_over_current_data->ns_cnt, channel, pkg_num, i, pk_data, PACKET_DATA_LEN);
                }
                msg_send_pack(data, len);
                delay_ms(100);
            }
        }
    }
}

void *data_send_serv(void *arg)
{
    (void)arg;
    msg_t msg;
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case PERIOD_DATA_TYPE:
            upload_period_data();
            break;
        case PF_CURVE_TYPE:

            break;
        case HF_CURVE_TYPE:
            send_over_current_curve();
            break;
        default:
            break;
        }
        delay_ms(100);
    }
}

#define DATA_SEND_PRIORITY  10
static char data_send_thread_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t data_send_serv_init(void)
{
    kernel_pid_t _pid = thread_create(data_send_thread_stack, sizeof(data_send_thread_stack),
                                      DATA_SEND_PRIORITY,
                                      THREAD_CREATE_STACKTEST, data_send_serv, NULL, "period data serv");
    period_data_hook(_pid);
    over_current_hook(_pid);
    return _pid;
}

