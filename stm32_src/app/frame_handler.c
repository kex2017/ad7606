#include "frame_handler.h"
#include <string.h>
#include "periph/rtt.h"
#include "log.h"
#include "frame_encode.h"
#include "frame_common.h"
#include "periph/rtt.h"
#include "ec20_at.h"
#include "periph/pm.h"
#include "timex.h"
#include "xtimer.h"

enum {
    READ = 0,
    WRITE
} ctrl_req_flag_t;

//static void delay_s(int s)
//{
//	xtimer_ticks32_t last_wakeup = xtimer_now();
//	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
//}
//
//static void delay_ms(int ms)
//{
//	xtimer_ticks32_t last_wakeup = xtimer_now();
//	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
//}


void frame_handler(frame_req_t *frame_req)
{
    switch (frame_req->func_code) {

    }
    return;
}



