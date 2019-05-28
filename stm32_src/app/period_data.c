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

static void upload_data(void)
{
    send_high_current_cycle_data();
    //发送其他周期数据
}

static uint32_t g_time_start = 0;
static uint32_t g_time_now = 0;
void *period_data_serv(void *arg)
{
    (void)arg;

    while (1) {
        g_time_now = rtt_get_counter();

        if (g_time_start == 0) {
            g_time_start = g_time_now;
        }
        if (g_time_now - g_time_start > cfg_get_device_data_interval()) {
            g_time_start = g_time_now;
            upload_data();
        }
        delay_ms(500);
    }
}

#define PERIOD_DATA_PRIORITY  9
static char period_data_thread_stack[THREAD_STACKSIZE_MAIN * 2];
kernel_pid_t period_data_serv_init(void)
{
    kernel_pid_t _pid = thread_create(period_data_thread_stack, sizeof(period_data_thread_stack),
                                      PERIOD_DATA_PRIORITY,
                                      THREAD_CREATE_STACKTEST, period_data_serv, NULL, "period data serv");
    return _pid;
}
