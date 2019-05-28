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

static kernel_pid_t data_send_pid;
void period_data_hook(kernel_pid_t pid)
{
    data_send_pid = pid;
}

void *period_data_serv(void *arg)
{
    (void)arg;
    msg_t msg;
    msg.type = PERIOD_DATA_TYPE;
    while (1) {
        delay_s(cfg_get_device_data_interval());
        msg_send(&msg, data_send_pid);
    }
}

#define PERIOD_DATA_PRIORITY  9
static char period_data_thread_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t period_data_serv_init(void)
{
    kernel_pid_t _pid = thread_create(period_data_thread_stack, sizeof(period_data_thread_stack),
                                      PERIOD_DATA_PRIORITY,
                                      THREAD_CREATE_STACKTEST, period_data_serv, NULL, "period data serv");
    return _pid;
}
