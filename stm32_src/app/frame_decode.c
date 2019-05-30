#include "frame_decode.h"
#include "log.h"

uint16_t frame_uint16_decode(uint8_t *data)
{
    return data[0] << 8 | data[1];
}

uint32_t frame_uint32_decode(uint8_t* data)
{
   uint32_t i = 0;
   uint32_t data_temp = 0;

   for (i = 0; i < 4; i++) {
      data_temp = (data_temp << 8) | *(data + i);
   }

   return data_temp;
}

uint16_t frame_cs_decode(uint8_t* data, uint8_t* cs)
{
    *cs = data[0];

    return 1;
}

uint16_t frame_func_code_decode(uint8_t* data, uint8_t* func_code)
{
    *func_code = data[0];

    return 1;
}

uint16_t frame_header_decode(uint8_t* data, frame_header_t* header)
{
   header->starter1 = data[0];
   header->slave_addr = frame_uint16_decode(data + 1);
   header->master_addr = frame_uint16_decode(data + 3);
   header->data_len = frame_uint16_decode(data + 5);

   return FRAME_HEADER_LEN;
}

uint16_t frame_time_ctrl_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.time_info.type = frame_data[0];
	master_frame_req->frame_req.time_info.timestamp = frame_uint32_decode(frame_data+1);

	return sizeof(uint8_t)+sizeof(uint32_t);
}

uint16_t frame_on_need_decode(void)
{
    return 0;
}

uint16_t frame_set_channel_info_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.channel_info.channel = frame_data[0];
	master_frame_req->frame_req.channel_info.threshold = frame_uint16_decode(frame_data+1);
	master_frame_req->frame_req.channel_info.change_rate = frame_uint16_decode(frame_data+3);

	return sizeof(uint8_t)+sizeof(uint32_t);
}

uint16_t frame_request_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.requset_data.channel = frame_data[0];
	master_frame_req->frame_req.requset_data.type = frame_data[1];
	return sizeof(uint16_t);
}

uint16_t frame_collection_cycle_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.collection_cycle.type= frame_data[0];
	master_frame_req->frame_req.collection_cycle.cycle = frame_uint16_decode(frame_data+1);
	return sizeof(uint16_t)+sizeof(uint8_t);
}

uint16_t frame_req_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint32_t index = 0;
	uint16_t ret = 0;

	index += frame_func_code_decode(frame_data + index, &master_frame_req->func_code);
	switch (master_frame_req->func_code) {
	case TIME_CTRL_REQ:
		ret = frame_time_ctrl_decode(frame_data + index, master_frame_req);
		break;
	case GET_RUNNING_STATE_REQ:
		ret = frame_on_need_decode();
		break;
	case GET_CHANNEL_INFO_REQ:
		ret = frame_on_need_decode();
		break;
	case SET_CHANNEL_INFO_REQ:
		ret = frame_set_channel_info_decode(frame_data + index, master_frame_req);
		break;
	case GET_DEV_INFO_REQ:
		ret = frame_on_need_decode();
		break;
	case REBOOT_ARM_REQ:
		ret = frame_on_need_decode();
		break;
	case SERVER_REQUEST_DATA_REQ:
		ret = frame_request_data_decode(frame_data + index, master_frame_req);
		break;
	case COLLECTION_CYCLE_REQ:
		ret = frame_collection_cycle_decode(frame_data + index, master_frame_req);
		break;
	default:
		LOG_ERROR("Receive error command type %02x", master_frame_req->func_code);
		break;
	}
	return ret;
}


uint16_t frame_decode(uint8_t *frame_data, frame_req_t *master_frame_req)
{
    uint16_t index = 0;
    index += frame_header_decode(frame_data, &master_frame_req->header);
    index += frame_req_data_decode(frame_data + index, master_frame_req);
    index += frame_cs_decode(frame_data + index, &master_frame_req->cs);

    return index;
}

