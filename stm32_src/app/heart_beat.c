#include "heart_beat.h"
#include "frame_encode.h"
#include "thread.h"
#include "periph/rtt.h"
#include "frame_common.h"
#include "board.h"
#include "timex.h"
#include "log.h"
#include "xtimer.h"
#include "periph/stm32f10x_std_periph.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

//void do_send_heart_beat_packet(uint8_t *data, uint32_t data_len)
//{
//	if (get_ec20_link_flag() == LINK_UP) {
//		while (set_data_to_cache(data, data_len) == 0) {
//			delay_ms(500);
//		}
//	}
//}


void *send_heart_beat_handler(void* arg)
{
	(void) arg;
	while (1) {
		delay_s(60);
//		send_sensor_heart_beat();
	}
}

#define SEND_HERRT_BEAT_PRIORITY (8)
static char send_heart_beat_thread_stack[THREAD_STACKSIZE_MAIN];
void send_heart_beat_thread_init(void) {
	DEBUG("Send heart beat thread start....\r\n");
	kernel_pid_t _pid;
	_pid = thread_create(send_heart_beat_thread_stack,
			sizeof(send_heart_beat_thread_stack), SEND_HERRT_BEAT_PRIORITY,
			THREAD_CREATE_STACKTEST, send_heart_beat_handler, NULL,
			"send_heart_beat");
	(void) _pid;
}


