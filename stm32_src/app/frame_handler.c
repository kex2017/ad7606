#include "frame_handler.h"
#include <string.h>
#include "periph/rtt.h"
#include "log.h"
#include "frame_encode.h"
#include "frame_common.h"
#include "internal_ad_sample.h"
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

	length = frame_get_running_state_encode(data, DEVICEOK, temp, humidity, voltage, work, rtt_get_counter());
	(void) length;
}

void get_channel_info_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	channel_info_t channel_info[4];
	for(int i = 0; i< 2; i++){
		channel_info[i].threshold = cfg_get_device_high_channel_threshold(i);
		channel_info[i].change_rate = cfg_get_device_high_channel_changerate(i);
	}
	for (int i = 0; i < 2; i++) {
		channel_info[i+2].threshold =   cfg_get_device_channel_threshold(i);
		channel_info[i+2].change_rate = cfg_get_device_channel_changerate(i);
	}
	length = frame_channel_info_encode(data, DEVICEOK, channel_info);

	msg_send_pack(data, length);
}


void set_chennel_info_by_type(channel_info_t * channel_info)
{
	if (channel_info->type == 0) {
		cfg_set_high_device_threshold(channel_info->channel,
				channel_info->threshold);
		cfg_set_high_device_changerate(channel_info->channel,
				channel_info->change_rate);
	} else {
		cfg_set_device_threshold(channel_info->channel,
				channel_info->threshold);
		cfg_set_device_changerate(channel_info->channel,
				channel_info->change_rate);
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
	float k ,b;

	for(int i = 0; i< 2; i++)
	{
		k = frame_req->frame_req.calibration_info.cal_data[i*2];
		b = frame_req->frame_req.calibration_info.cal_data[i*2+1];
		cfg_set_device_k_b(i, k, b);
	}
	for(int i = 0; i< 2; i++)
	{
		k = frame_req->frame_req.calibration_info.cal_data[i*2+4];
		b = frame_req->frame_req.calibration_info.cal_data[i*2+5];
		cfg_set_high_device_k_b(i, k, b);
	}

	length = frame_set_calibration_info_encode(data, DEVICEOK, rtt_get_counter());

	msg_send_pack(data, length);
}

void get_calibration_info_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	calibration_info_t calibration_info;
	calibration_data_t *calibration_data[4];

	for(int i = 0; i< 2; i++){
		calibration_data[i] = cfg_get_calibration_k_b(i);
		calibration_info.cal_data[i*2] = calibration_data[i]->k;
		calibration_info.cal_data[i*2+1] = calibration_data[i]->b;
	}
	for (int i = 0; i < 2; i++) {
		calibration_data[i+2] = cfg_get_high_calibration_k_b(i);
		calibration_info.cal_data[i*2+4] = calibration_data[i+2]->k;
		calibration_info.cal_data[i*2+5] = calibration_data[i+2]->b;
	}

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



