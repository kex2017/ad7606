#include "frame_handler.h"
#include <string.h>
#include "periph/rtt.h"
#include "log.h"
#include "frame_encode.h"
#include "frame_common.h"
#include "periph/rtt.h"
#include "ec20_at.h"
#include "periph/pm.h"
#include "type_alias.h"
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

void time_ctrl_handler(frame_req_t *frame_req)
{
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	uint16_t length = 0;

	time_info_t * time_info = &frame_req->frame_req.time_info;

	if(time_info->type)
	{
		LOG_INFO("Receive set dev time command");
		rtt_set_counter(time_info->timestamp);
	}else
	{
		LOG_INFO("Receive get dev time command");
	}
	length = frame_time_ctrl_data_encode(data, DEVICEOK, rtt_get_counter());
	(void) length;
//	msg_send_pack(data, length);
}


void get_running_state_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	uint32_t temp =0 , humidity = 0, voltage = 0;
	uint8_t work = 0;
//	temp = get_temp();
//	humidity = get_humidity();
//	voltage = get_voltage();

	length = frame_get_running_state_encode(data, DEVICEOK, temp, humidity, voltage, work, rtt_get_counter());
	(void) length;
}

void frame_handler(frame_req_t *frame_req)
{
	switch (frame_req->func_code) {
	case TIME_CTRL_REQ:
		time_ctrl_handler(frame_req);
		break;
	case GET_RUNNING_STATE_REQ:
		LOG_INFO("Receive get running state command");
		get_running_state_handler();
		break;
	case GET_CHANNEL_INFO_REQ:
		break;
	case SET_CHANNEL_INFO_REQ:
		break;
	case GET_DEV_INFO_REQ:
		break;
	default:
		break;
	}
	return;
}



