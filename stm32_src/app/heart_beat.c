#include "heart_beat.h"
#include "fault_location_threads.h"
#include "frame_encode.h"
#include "thread.h"
#include "periph/rtt.h"
#include "frame_common.h"
#include "data_transfer.h"
#include "frame_handler.h"
#include "board.h"
#include "x_delay.h"
#include "type_alias.h"
#include "log.h"
#include "periph/stm32f10x_std_periph.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

void do_send_heart_beat_packet(uint8_t *data, uint32_t data_len)
{
	if (get_ec20_link_flag() == LINK_UP) {
		while (set_data_to_cache(data, data_len) == 0) {
			delay_ms(500);
		}
	}
}

void time_to_send_heart_msg(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };

	length = frame_heart_beat_encode(data, DEVICEOK, rtt_get_counter());

	do_send_heart_beat_packet(data, length);
}


void *send_heart_beat_handler(void* arg)
{
 	(void) arg;
 	static int times = 0;
 	while (1) {
 		delay_s(60);
		if(times == 10)
		{
			do_send_dev_info_msg();
		}
		times ++;
		LOG_INFO("Keep alive");
		time_to_send_heart_msg();
 	}
}

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


