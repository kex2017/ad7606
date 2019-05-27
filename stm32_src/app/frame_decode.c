#include "frame_decode.h"

uint16_t frame_uint16_decode(uint8_t *data)
{
    return data[1] << 8 | data[0];
}

uint32_t frame_uint32_decode(uint8_t* data)
{
    return (data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]);
}

uint16_t frame_header_decode(uint8_t* frame_data, frame_header_t* frame_header)
{
	frame_header->sync =  frame_uint16_decode(frame_data);
	frame_header->packet_length = frame_uint16_decode(frame_data+2);
	memcpy(frame_header->cmd_id,frame_data+4,17);
	return FRAME_DECODE_HEAED_LEN;
}

uint16_t frame_type_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_type = frame_data[0];

	return sizeof(uint8_t);
}

uint16_t frame_time_ctrl_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;
	 master_frame_req->frame_req.device_time.type = frame_data[0];
     index += frame_set_time_req_decode(frame_data + 1,  master_frame_req);

     return index+1;
}

uint16_t frame_ip_addr_decode(uint8_t *data, frame_req_t* req)
{
	for (int i = 0; i < 4; i++) {
		req->frame_req.net_dev.ip_addr[i] = data[i];
	}
	return sizeof(uint32_t);
}

uint16_t frame_net_mask_decode(uint8_t *data, frame_req_t* req)
{
	for (int i = 0; i < 4; i++) {
		req->frame_req.net_dev.net_mask[i] = data[i];
	}
	return sizeof(uint32_t);
}

uint16_t frame_gateway_decode(uint8_t *data, frame_req_t* req)
{
	for (int i = 0; i < 4; i++) {
		req->frame_req.net_dev.gateway[i] = data[i];
	}
	return sizeof(uint32_t);
}

uint16_t frame_dns_decode(uint8_t *data, frame_req_t* req)
{
	for (int i = 0; i < 4; i++) {
		req->frame_req.net_dev.dns[i] = data[i];
	}
	return sizeof(uint32_t);
}

uint16_t frame_net_dev_decode(uint8_t *data, frame_req_t* req)
{
	uint16_t index = 0;

	index += frame_ip_addr_decode(data+index,  req);
	index += frame_net_mask_decode(data+index,req);
	index += frame_gateway_decode(data+index, req);
	index += frame_dns_decode(data+index, req);

	return (index+16);//16位备用长度
}


uint16_t frame_net_ctrl_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 1;

	master_frame_req->frame_req.net_dev.type = frame_data[0];
    index += frame_net_dev_decode(frame_data + index,  master_frame_req);

     return index;
}

uint16_t frame_one_threshold_cfg_decode(uint8_t* frame_data, frame_req_t *master_frame_req, int num)
{
	uint16_t index = 0;

	for(int i = 0; i<6; i++)
	{
		 master_frame_req->frame_req.threshold_cfg.thr_data[num].warn_cfg[i] = frame_data[index++];
		 index++;
	}
	 master_frame_req->frame_req.threshold_cfg.thr_data[num].threshold = frame_uint32_decode(frame_data+index);

	 return 10;
}

uint16_t frame_threshold_cfg_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;

	for(int i =0; i<master_frame_req->frame_req.threshold_cfg.sum; i++)
	{
			index += frame_one_threshold_cfg_decode(frame_data+index, master_frame_req, i);
	}

	return index;
}

uint16_t  frame_threshold_ctrl_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;

	master_frame_req->frame_req.threshold_cfg.req_type = frame_data[0];
	master_frame_req->frame_req.threshold_cfg.data_type = frame_data[1];
	master_frame_req->frame_req.threshold_cfg.sum = frame_data[2];
	index += frame_threshold_cfg_decode(frame_data+3,master_frame_req);

	return index+sizeof(uint8_t )*3;
}

uint16_t frame_sever_ip_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	for (int i = 0; i < 4; i++) {
		master_frame_req->frame_req.server_cfg.ip[i] = frame_data[i];
	}
	return sizeof(uint32_t);
}

uint16_t frame_sever_port_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.server_cfg.port = frame_uint16_decode(frame_data);
	return sizeof(uint16_t);
}

uint16_t frame_domain_main_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	for(int i= 0; i< 64 ; i++)
	{
		master_frame_req->frame_req.server_cfg.domain[i] = frame_data[i];
	}
	return 64;
}

uint16_t superior_request_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.request_data.request_type = frame_data[0];
	master_frame_req->frame_req.request_data.start_time = frame_uint32_decode(frame_data+1);
	master_frame_req->frame_req.request_data.end_time = frame_uint32_decode(frame_data+5);

	return sizeof(uint32_t)*2+1;
}


uint16_t frame_dev_config_decode(uint8_t* frame_data,  frame_req_t *master_frame_req)
{
	if (frame_data[0] == SET_SUCCESS && frame_data[1] == (0x01)) {
		LOG_INFO("Server receive device config message success");
	} else if (frame_data[0] == SET_ERROR) {
		LOG_INFO("Server receive device config message error");
	}
	master_frame_req->frame_req.dev_register.msg_send_flag = frame_data[0] ;
	master_frame_req->frame_req.dev_register.register_flag = frame_data[1] ;
	return 2;
}

uint16_t frame_condition_type_decode(uint8_t* frame_data)
{
	if (frame_data[0] == SET_SUCCESS) {
		LOG_INFO("Server receive condition message success");
	} else if (frame_data[0] == SET_ERROR) {
		LOG_INFO("Server receive condition message error");
	}
	return 1;
}

uint16_t frame_fault_msg_decode(uint8_t* frame_data)
{
	if (frame_data[0] == SET_SUCCESS) {
		LOG_INFO("Server receive fault message success");
	} else if (frame_data[0] == SET_ERROR) {
		LOG_INFO("Server receive fault message error");
	}
	return 1;
}

uint16_t frame_server_cfg_select_req_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;
	for(int i= 0; i< 30; i++)
	{
		printf("%02x ", frame_data[i]);
	}
	printf("\r\n");
	master_frame_req->frame_req.server_cfg.req_type = frame_data[index++];
	index += frame_sever_ip_decode(frame_data+index,master_frame_req);
	index += frame_sever_port_decode(frame_data+index,master_frame_req);
	index += frame_domain_main_decode(frame_data+index,master_frame_req);

	return index;
}

uint16_t frame_dev_cfg_select_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.dev_cfg_select.req_type = frame_data[0];
	master_frame_req->frame_req.dev_cfg_select.data_type = frame_data[1];
	master_frame_req->frame_req.dev_cfg_select.msg_type = frame_data[2];

	return sizeof(uint8_t)*3;
}

uint16_t frame_file_name_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;
	for (; index < 20; index++) {
		master_frame_req->frame_req.transfer_file_req.file_name[index] = frame_data[index];
	}
	return 20;
}

uint16_t frame_upgrade_pack_decode(uint8_t* frame_data, frame_req_t *master_frame_req,uint8_t type)
{
	if (type == 1) {
		master_frame_req->frame_req.transfer_file_req.file_count =
				frame_uint32_decode(frame_data);
	} else {
		master_frame_req->frame_req.transfer_file_req.file_index =
				frame_uint32_decode(frame_data);
	}
	return sizeof(uint32_t);
}

uint16_t frame_file_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;

	for(; index<master_frame_req->header.packet_length - 28; index++)
	{
		master_frame_req->frame_req.transfer_file_req.buff[index] = frame_data[index];
	}
	master_frame_req->frame_req.transfer_file_req.data_len = master_frame_req->header.packet_length - 28;

	return index;
}

uint16_t frame_app_upgrade_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;

	master_frame_req->frame_req.transfer_file_req.file_type = 1;
	index += frame_file_name_decode(frame_data+index, master_frame_req);
	master_frame_req->frame_req.transfer_file_req.file_count =frame_uint32_decode(frame_data+index);
	master_frame_req->frame_req.transfer_file_req.file_index = frame_uint32_decode(frame_data+index+4);
	index += frame_file_data_decode(frame_data+index+8, master_frame_req);

	return index+8;
}

uint16_t frame_component_id_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t  index =0;
	for(; index<17;index++)
	{
		master_frame_req->frame_req.component_dev.id[index] = frame_data[index];
	}
	return index;
}

uint16_t frame_component_dev_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 1;

	master_frame_req->frame_req.component_dev.req_type = frame_data[0];
	index += frame_component_id_decode( frame_data+index,master_frame_req);
	master_frame_req->frame_req.component_dev.original_id = frame_uint16_decode(frame_data+index);

	return index+sizeof(uint16_t);
}

uint16_t frame_dev_reset_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.dw_commstatus = frame_uint16_decode(frame_data );
	return sizeof(uint16_t);
}
uint8_t frame_sample_ctrl_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	uint16_t index = 0;
	master_frame_req->frame_req.sample_param.request_set_flag = frame_data [index];
	index += sizeof(uint8_t);
	master_frame_req->frame_req.sample_param.request_type = frame_data [index];
	index += sizeof(uint8_t);
	master_frame_req->frame_req.sample_param.main_time = frame_uint16_decode(frame_data + index);
	index += sizeof(uint16_t);
	master_frame_req->frame_req.sample_param.sample_count = frame_uint16_decode(frame_data + index);
	index += sizeof(uint16_t);
	master_frame_req->frame_req.sample_param.sample_frequency = frame_uint16_decode(frame_data + index);
	index += sizeof(uint16_t);
	master_frame_req->frame_req.sample_param.reserve = frame_uint32_decode(frame_data + index);
	index += sizeof(uint32_t);
	return index;
}

uint8_t frame_monitor_data_upload_rsp_encode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    master_frame_req->frame_req.w_commstatus = frame_data[0];
    return sizeof(uint8_t);
}

uint8_t frame_set_revival_time_decode (uint8_t* frame_data, frame_req_t *master_frame_req)
{	
	uint16_t index = 0;
	master_frame_req->frame_req.revival_time.ref_rev_time = frame_uint32_decode(frame_data + index);
	index += sizeof(uint32_t);
	master_frame_req->frame_req.revival_time.rev_crcle = frame_uint16_decode(frame_data + index);
	index += sizeof(uint16_t);
	master_frame_req->frame_req.revival_time.d_time = frame_uint16_decode(frame_data + index);
	index += sizeof(uint16_t);
	master_frame_req->frame_req.revival_time.reserve = frame_uint16_decode(frame_data + index);
	index += sizeof(uint32_t);

	return index;

}

uint8_t  send_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.w_commstatus = frame_data[0];
	return sizeof(uint8_t);
}

uint16_t frame_set_or_get_dev_ip(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->frame_req.dev_ip.req_type = frame_data[0];
	for(int i = 0;i<16;i++)
	{
		master_frame_req->frame_req.dev_ip.no[i] = frame_data[i+1];
	}
	return 17;
}

uint32_t monitor_data_frame_type_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    uint32_t index = 0;
    switch (master_frame_req->msg_type) {
    case CUR_DATA_UPLOAD_PKT:
    	send_data_decode(frame_data,master_frame_req);
    	break;
    case ALARM_STATUS_UPLOAD_PKT:
    	send_data_decode(frame_data,master_frame_req);
    	break;
    case VOL_DATA_UPLOAD_PKT:
    	send_data_decode(frame_data,master_frame_req);
    	break;
    case HFCT_DATA_UPLOAD_PKT:
    	send_data_decode(frame_data,master_frame_req);
    	break;
    case TEMP_DATA_UPLOAD_PKT:
    	send_data_decode(frame_data,master_frame_req);
    	break;
    case SHORT_CIRCUIT_ALARM_UPLOAD_PKT:
        index += frame_monitor_data_upload_rsp_encode(frame_data, master_frame_req);
        break;
    default:
        LOG_INFO("[Frame decode]:Get invalid message type: %02XH, please check again.", master_frame_req->msg_type);
        break;
    }
    return index;
}

uint16_t control_frame_type_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    uint16_t index = 0;
	switch (master_frame_req->msg_type){
	case CTRL_TIME_PKT:
	    index += frame_time_ctrl_decode(frame_data+index,master_frame_req);
			break;
	case  CTRL_DEV_NET_INFO_PKT:
		 index += frame_net_ctrl_decode(frame_data+index,master_frame_req);
		break;
	case  SUPERIOR_GET_DATA_PACK_TYPE:
		index += superior_request_data_decode(frame_data+index,master_frame_req);
		break;
	case  CTRL_SAMPLE_INFO_PKT:
	    	index += frame_sample_ctrl_decode(frame_data,master_frame_req);
		break;
	case MODEL_CONFIG_SET_PACK_TYPE:
		break;
	case CTRL_THRESHOLD_PKT:
		index += frame_threshold_ctrl_decode(frame_data+index,master_frame_req);
		break;
	case CTRL_SERVER_NET_INFO_PKT:
		index += frame_server_cfg_select_req_decode(frame_data+index,master_frame_req);
		break;
	case CTRL_DEV_CFG_PKT:
		index += frame_dev_cfg_select_decode(frame_data+index,master_frame_req);
		break;
	case CTRL_DEV_UPGRADE_TRANSFER_PKT:
	case CTRL_DEV_UPGRADE_CNF_PKT://TODO: decode value
		index += frame_app_upgrade_decode(frame_data+index,master_frame_req);
		break;
	case COMPONENT_DEV_PACK_TYPE:
		index += frame_component_dev_decode(frame_data+index,master_frame_req);
		break;
	case CTRL_DEV_RESET_PKT:
		index += frame_dev_reset_decode(frame_data+index,master_frame_req);
		break;
	case SET_REVIVAL_TIME_PAKC_TYPE:
		index += frame_set_revival_time_decode(frame_data+index,master_frame_req);
		break;
	case CTRL_DEV_IP_PACK_TYPE:
		index += frame_set_or_get_dev_ip(frame_data+index,master_frame_req);
		break;
	default:
		LOG_INFO("Get invalid message type: %02XH, please check again.", master_frame_req->msg_type);
	break;
}
	return index;
}

uint16_t work_conditin_type_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    uint16_t index = 0;
	switch (master_frame_req->msg_type){
	case DEV_CONFIG_PACK_TYPE:
		index += frame_dev_config_decode(frame_data+index,  master_frame_req);
		break;
	case WORK_CONDITION_PACK_TYPE:
		index += frame_condition_type_decode(frame_data+index);
		break;
	case FAULT_MSG_PACK_TYPE:
		index += frame_fault_msg_decode(frame_data+index);
		break;
	default:
		LOG_ERROR("Error CMD for 0x08,Please check");
		break;
	}
	return index;
}

uint16_t msg_type_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
	master_frame_req->msg_type = frame_data[0];

	return sizeof(uint8_t);
}

uint16_t frame_req_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    uint16_t index = 0;

    index += frame_type_decode(frame_data+index,master_frame_req);
    index += msg_type_decode(frame_data+index,master_frame_req);
    switch(master_frame_req->frame_type)
    {
    case MONITOR_RSP_FRAME:
   	     index += monitor_data_frame_type_decode( frame_data+index, master_frame_req);
   		 break;
   	 case CONTROL_DATA_FRAME:
   	     index += control_frame_type_decode( frame_data+index, master_frame_req);
   		 break;
   	 case WORK_CONDITIN_REQ_FRAME:
   		 index += work_conditin_type_decode( frame_data+index, master_frame_req);
   		 break;
   	 case 0x07:
   		 break;
   	 default:
   		 LOG_INFO("Get invalid frame type: %02XH, please check again.", master_frame_req->frame_type);
   		   break;
    }
    return index;
}


uint16_t frame_cs_decode(uint8_t* data, uint16_t* checksum)
{
    *checksum = frame_uint16_decode(data);

    return sizeof(unsigned short);
}

uint16_t frame_set_time_req_decode(uint8_t *data, frame_req_t* req)
{
    req->frame_req.device_time.time = frame_uint32_decode(data);
    return sizeof(uint32_t);
}

uint16_t frame_decode(uint8_t *frame_data, frame_req_t *master_frame_req)
{
    uint16_t index = 0;

    index += frame_header_decode(frame_data, &master_frame_req->header);
    index += frame_req_data_decode(frame_data + index,  master_frame_req);
    index += frame_cs_decode(frame_data + index, &master_frame_req->cs);

    return index;
}

