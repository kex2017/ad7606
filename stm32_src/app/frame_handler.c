#include "frame_handler.h"
#include <string.h>
#include "periph/rtt.h"
#include "log.h"
#include "frame_encode.h"
#include "frame_common.h"
#include "data_transfer.h"
#include "over_current.h"
#include "periph/rtt.h"
#include "ec20_at.h"
#include "env_cfg.h"
#include "periph/pm.h"
#include "type_alias.h"
#include "x_delay.h"

enum {
    READ = 0,
    WRITE
} ctrl_req_flag_t;

void msg_send_pack(uint8_t *data, uint32_t data_len)
{
	if (get_ec20_link_flag() == LINK_UP) {
		while (set_data_to_cache(data, data_len) == 0) {
			delay_ms(500);
		}
	}
}

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

	msg_send_pack(data, length);
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

void get_channel_info_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	over_current_info_t * over_current_info[2];
	channel_info_t channel_info[4];
	for (int i = 0; i < 2; i++) {
		over_current_info[i] = get_over_current_info(i);
		channel_info[i].threshold = over_current_info[i]->threshold;
		channel_info[i].change_rate = over_current_info[i]->change_rate;
	}
//FIXME: 工频缺失
	length = frame_channel_info_encode(data, DEVICEOK, channel_info);

	msg_send_pack(data, length);
}


void set_chennel_info_by_type(channel_info_t * channel_info)
{
		if(channel_info->type == 0)
		{
			set_over_current_threshold(channel_info->channel, channel_info->threshold);
			set_over_current_changerate(channel_info->channel, channel_info->change_rate);
		}else{
			//FIXME: 工频数据缺失
//			set_power_current_info(channel_info->channel, channel_info->threshold, channel_info->change_rate);
		}

}

void set_channel_info_handler(frame_req_t *frame_req)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};

	set_chennel_info_by_type(&frame_req->frame_req.channel_info);

	length = frame_set_channel_info_rsp_encode(data, DEVICEOK, rtt_get_counter());

	msg_send_pack(data, length);
}


void get_dev_info_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };

	length = dev_info_encode(data, DEVICEOK,
			(uint8_t *) cfg_get_device_version(),
			(uint16_t) strlen(cfg_get_device_version()),
			cfg_get_device_longitude(), cfg_get_device_latitude(),
			cfg_get_device_height());

	msg_send_pack(data, length);
}

void do_send_dev_info_msg(void)
{
	get_dev_info_handler();
}

void set_calibration_info_handler(frame_req_t *frame_req)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	cal_k_b_t cal_k_b[2];

	for(int i = 0; i< 2; i++)
	{
		cal_k_b[i].k = frame_req->frame_req.calibration_info.cal_data[i*2+4];
		cal_k_b[i].b = frame_req->frame_req.calibration_info.cal_data[i*2+5];
		 set_over_current_cal_k_b(i, cal_k_b[i]);
	}

	length = frame_set_calibration_info_encode(data, DEVICEOK, rtt_get_counter());

	msg_send_pack(data, length);
}

void get_calibration_info_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	cal_k_b_t over_current[2];
	calibration_info_t calibration_info;
	for (int i = 0; i < 2; i++) {
		over_current[i] = get_over_current_cal_k_b(i);
		calibration_info.cal_data[i*2] = over_current[i].k;
		calibration_info.cal_data[i*2+1] = over_current[i].b;
	}
	//FIXME: 工频数据缺失

	length = frame_get_calibration_info_encode(data, DEVICEOK,&calibration_info);

	msg_send_pack(data, length);
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
		LOG_INFO("Receive get channel info command");
		get_channel_info_handler();
		break;
	case SET_CHANNEL_INFO_REQ:
		LOG_INFO("Receive set channel info command");
		set_channel_info_handler(frame_req);
		break;
	case GET_DEV_INFO_REQ:
		LOG_INFO("Receive get dev info command");
		get_dev_info_handler();
		break;
    case SET_CALIBRATION_INFO_REQ:
    	LOG_INFO("Receive set calibration info command");
    	set_calibration_info_handler(frame_req);
    	break;
    case GET_CALIBRATION_INFO_REQ:
    	LOG_INFO("Receive get calibration info command");
    	get_calibration_info_handler();
    	break;
	default:
		break;
	}
	return;
}



