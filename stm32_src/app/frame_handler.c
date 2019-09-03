#include "frame_handler.h"
#include <string.h>
#include "periph/rtt.h"
#include "log.h"
#include "frame_encode.h"
#include "frame_common.h"
#include "upgrade_from_flash.h"
#include "app_upgrade.h"
#include "data_transfer.h"
#include "periph/rtt.h"
#include "ec20_at.h"
#include "env_cfg.h"
#include "data_send.h"
#include "over_current.h"
#include "periph/pm.h"
#include "type_alias.h"
#include "x_delay.h"

enum
{
	READ = 0,
	WRITE
} ctrl_req_flag_t;

void msg_send_pack(uint8_t *data, uint32_t data_len)
{
//    printf("send to server frame:\r\n");
//    for(uint32_t i = 0; i < data_len; i++){
//        printf("%02x ", data[i]);
//    }
//    printf("\r\n\r\n");

	if (get_ec20_link_flag() == LINK_UP)
	{
		while (set_data_to_cache(data, data_len) == 0)
		{
			delay_ms(500);
		}
	}
}

void time_ctrl_handler(frame_req_t *frame_req)
{
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	uint16_t length = 0;

	time_info_t *time_info = &frame_req->frame_req.time_info;

	if (time_info->type)
	{
		LOG_INFO("Receive set dev time command");
		rtt_set_counter(time_info->timestamp);
	}
	else
	{
		LOG_INFO("Receive get dev time command");
	}
	length = frame_time_ctrl_data_encode(data, DEVICEOK, time_info->type, rtt_get_counter());

	msg_send_pack(data, length);
}

void get_running_state_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	uint32_t temp = 0, humidity = 0, voltage = 0;
	uint8_t work = 0;

	length = frame_get_running_state_encode(data, DEVICEOK, temp, humidity, voltage, work, rtt_get_counter());
	(void)length;
}

void get_channel_info_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };

    uint8_t logic_channel = frame_req->frame_req.channel_info.channel;

    uint8_t phase = logic_channel / 4;
    uint8_t channel = logic_channel % 4;
    channel_info_t channel_info;

    channel_info.channel = logic_channel;
    if (channel < 2) { //pf
        channel_info.threshold = cfg_get_device_pf_channel_threshold(phase, channel);
        channel_info.change_rate = cfg_get_device_pf_channel_changerate(phase, channel);
    }
    else {
        channel_info.threshold = cfg_get_device_hf_channel_threshold(phase, channel);
        channel_info.change_rate = cfg_get_device_hf_channel_changerate(phase, channel);
    }
    length = frame_channel_info_encode(data, DEVICEOK, &channel_info);

    msg_send_pack(data, length);
}

void set_channel_info_by_type(channel_info_t *channel_info)
{
    uint8_t logic_channel = channel_info->channel;
    uint8_t channel = logic_channel % 4;
    uint8_t phase = logic_channel / 4;

    if (channel_info->channel > 1) {
        cfg_set_high_device_threshold(phase, channel, channel_info->threshold);
        set_hf_over_current_threshold(phase, channel, channel_info->threshold);
        cfg_set_high_device_changerate(phase, channel, channel_info->change_rate);
        set_hf_over_current_changerate(phase, channel, channel_info->change_rate);
    }
    else {
        cfg_set_device_threshold(phase, channel, channel_info->threshold);
        cfg_set_device_changerate(phase, channel, channel_info->change_rate);
        set_pf_over_current_threshold(phase, channel, channel_info->threshold);
    }
}

void set_channel_info_handler(frame_req_t *frame_req)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	set_channel_info_by_type(&frame_req->frame_req.channel_info);

	length = frame_set_channel_info_rsp_encode(data, DEVICEOK, frame_req->frame_req.channel_info.channel, rtt_get_counter());

	msg_send_pack(data, length);
}

void get_dev_info_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};

	length = dev_info_encode(data, DEVICEOK,
							 (uint8_t *)cfg_get_device_version(),
							 (uint16_t)strlen(cfg_get_device_version()),
							 cfg_get_device_longitude(), cfg_get_device_latitude(),
							 cfg_get_device_height());

	msg_send_pack(data, length);
}

void do_send_dev_info_msg(void)
{
	get_dev_info_handler();
	send_dip_angle_data();
	delay_ms(200);
}

void reboot_handler(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	length = frame_reboot_encode(data, DEVICEOK, rtt_get_counter());

	msg_send_pack(data, length);

	delay_s(1);
	soft_reset();
}

static kernel_pid_t data_send_pid;
void request_data_hook(kernel_pid_t pid)
{
	data_send_pid = pid;
}

void server_request_data_handler(frame_req_t *frame_req)
{
    msg_t msg;
    if (frame_req->frame_req.requset_data.type) {//召唤波形
        trigger_sample_over_current_by_hand();
        LOG_INFO("server call wave data");
    }
    else {                                       //召唤有效值
        msg.type = CALL_DATA_TYPE;
        msg.content.value = rtt_get_counter();
        msg_send(&msg, data_send_pid);
        LOG_INFO("server call effective value data");
    }
}

void upload_file_req_handler(frame_req_t *frame_req)
{
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	uint16_t file_count = 0;
	uint16_t file_index = 0;
	uint8_t file_type = 0;
	uint8_t last_packet = 1;
	uint16_t length = 0;
	uint8_t errcode = DEVICEOK;

	file_count = frame_req->frame_req.transfer_file_req.file_count;
	file_index = frame_req->frame_req.transfer_file_req.file_index;
	file_type = frame_req->frame_req.transfer_file_req.file_type;

	if (0 == file_index)
	{
		LOG_DEBUG("Start to upload arm program file, during file transfer, ignore new data.");
		set_arm_file_transfer_flag(SD_TRUE);
	}

	if (file_index >= file_count)
	{
		LOG_WARN("upload file request frame invalid: file_index(%d) >= file_count(%d)", file_index, file_count);
		errcode = TRANSFERERR;
		set_arm_file_transfer_flag(SD_FALSE);
	}
	else if (SD_TRUE != process_upload_file_req(&frame_req->frame_req.transfer_file_req, &last_packet))
	{
		LOG_ERROR("process upload file request failed!, file_index(%d), file_count(%d), file_type(%d)", file_index,
				  file_count, file_type);
		errcode = TRANSFERERR;
		set_arm_file_transfer_flag(SD_FALSE);
	}
	LOG_INFO("Upload file pkt info: type:%d, total:%d, index:%d, last_flag:%d", file_type, file_count, file_index,
			 last_packet);
	length = frame_transfer_file_rsp_encode(data, errcode, frame_req->frame_req.transfer_file_req.file_type, file_index);
	msg_send_pack(data, length);
	if (SD_TRUE == last_packet)
	{
		process_upgrade_programe_req(frame_req->frame_req.transfer_file_req.md5, file_type);
	}
}

void collection_cycle_handler(frame_req_t *frame_req)
{
	uint16_t length = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = {0};
	uint16_t *p_interval = cfg_get_device_data_interval();

	if (frame_req->frame_req.collection_cycle.type)
	{
		LOG_INFO("Receive set collection cycle command");
		cfg_set_device_data_interval(frame_req->frame_req.collection_cycle.cycle);
	}
	else
	{
		LOG_INFO("Receive get collection cycle command");
	}

	length = frame_collection_cycle_data_encode(data, DEVICEOK, frame_req->frame_req.collection_cycle.type, *p_interval);

	msg_send_pack(data, length);
}

void frame_handler(frame_req_t *frame_req)
{
	switch (frame_req->func_code)
	{
	case TIME_CTRL_REQ:
		time_ctrl_handler(frame_req);
		break;
	case GET_RUNNING_STATE_REQ:
		LOG_INFO("Receive get running state command");
		get_running_state_handler();
		break;
	case GET_CHANNEL_INFO_REQ:
		LOG_INFO("Receive get channel info command");
		get_channel_info_handler(frame_req);
		break;
	case SET_CHANNEL_INFO_REQ:
		LOG_INFO("Receive set channel info command");
		set_channel_info_handler(frame_req);
		break;
	case GET_DEV_INFO_REQ:
		LOG_INFO("Receive get dev info command");
		get_dev_info_handler();
		break;
	case REBOOT_ARM_REQ:
		LOG_INFO("Receive reboot command");
		reboot_handler();
		break;
	case SERVER_REQUEST_DATA_REQ:
		LOG_INFO("Receive request data command");
		server_request_data_handler(frame_req);
		break;
	case FRAME_TRANSFER_FILE_REQ:
		upload_file_req_handler(frame_req);
		break;
	case COLLECTION_CYCLE_REQ:
		collection_cycle_handler(frame_req);
		break;
	default:
		break;
	}
	return;
}
