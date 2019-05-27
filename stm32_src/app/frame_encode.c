#include "frame_encode.h"
#include "frame_common.h"
#include "app_upgrade.h"
#include "periph/rtt.h"
#include "string.h"
#include "bitfield.h"

uint16_t frame_uint8_encode(uint8_t *data,uint8_t value)
{
    data[0] = value;

    return sizeof(unsigned char);
}

uint16_t frame_uint16_encode(uint8_t *data,uint16_t value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[0];
    data[1] = p[1];

    return sizeof(unsigned short);
}

uint16_t frame_uint32_encode(uint8_t *data,uint32_t value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[0];
    data[1] = p[1];
    data[2] = p[2];
    data[3] = p[3];

    return sizeof(unsigned int);
}

uint16_t frame_float_encode(uint8_t *data,float value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[0];
    data[1] = p[1];
    data[2] = p[2];
    data[3] = p[3];

    return sizeof(float);
}

uint16_t frame_header_encode(uint8_t* out,uint16_t packet_len,uint8_t frame_type,uint8_t packet_type ,uint8_t * cmd_id)
{
    uint16_t index = 0;

    index += frame_uint16_encode(out + index,PACKET_SYNC);
    index += frame_uint16_encode(out + index, packet_len);
    for (int id_len = 0; id_len < CMD_ID_LEN; id_len++) {
        index += frame_uint8_encode(out + index,cmd_id[id_len]);
    }
    index += frame_uint8_encode(out + index, frame_type);
    index += frame_uint8_encode(out + index, packet_type);

    return index;
}

uint16_t frame_data_encode(uint8_t* in, uint16_t data_len, uint8_t* out)
{
    uint16_t index = 0;

    for (int i = 0; i < data_len; i++) {
        index += frame_uint8_encode(out + index, in[i]);
    }

    return index;
}

uint16_t frame_checksum_encode(uint8_t* frame_start, uint16_t front_len, uint8_t* out)
{
    uint16_t index = 0, check_sum = 0;

    check_sum = RTU_CRC(frame_start, front_len);
    index += frame_uint16_encode(out, check_sum);

    return index;
}

uint16_t frame_set_device_time_rsp_encode(uint8_t cmd_status, uint32_t cur_timestamp, uint8_t* out)
{
    uint16_t index = 0;

    index += frame_uint8_encode(out + index, cmd_status);
    index += frame_uint32_encode(out + index, cur_timestamp);

    return index;
}

uint16_t frame_heart_beat_rsp_encode(uint8_t *data, uint32_t timestamp, uint8_t * cmd_id){

	uint16_t index = 0;

	index += frame_header_encode(data+index, TIMESTAMP_DATA_LEN, WORK_CONDITION_RSP_FRAME, HEART_BEAT_PACK_TYPE ,cmd_id);
	index += frame_uint32_encode(data+index,timestamp);
	index += frame_checksum_encode(data,index,data+index);

	return index;
}

uint16_t frame_device_config_rsp_encode(uint8_t *data,product_info_t* dev_info,uint8_t* cmd_id) {

	uint16_t index = 0;

	int i;
	index +=frame_header_encode(data+index,DEVICE_CONFIG_DATA_LEN,WORK_CONDITION_RSP_FRAME,DEV_CONFIG_PACK_TYPE, cmd_id);
	for( i = 0; i<50;i++){
		index += frame_uint8_encode(data+index,dev_info->name[i]);
	}
	for(i = 0 ; i<10; i++)
	{
		index += frame_uint8_encode(data+index,dev_info->type[i]);
	}
	index += frame_float_encode(data+index,dev_info->version);
	for( i = 0; i<50;i++){
		index += frame_uint8_encode(data+index,dev_info->manufacture[i]);
	}
	index += frame_uint32_encode(data+index,dev_info->date);
	for(i = 0; i<20;i++)
	{
		index += frame_uint8_encode(data+index,dev_info->id[i]);
	}
	for(i = 0; i<30;i++)
	{
		index += frame_uint8_encode(data+index,0);
	}
	index += frame_checksum_encode(data,index,data+index);
	return index;
}

uint16_t frame_work_condition_rsp_encode(uint8_t *data, uint8_t *cmd_id,float voltage,float temp,float el_quantity ,uint8_t state,float sum_time,float work_time,uint8_t net_state,uint8_t csq)
{
    uint16_t index = 0;

    index += frame_header_encode(data + index, WORK_CONDITION_DATA_LEN, WORK_CONDITION_RSP_FRAME, WORK_CONDITION_PACK_TYPE, cmd_id);
    index += frame_uint32_encode(data + index, rtt_get_counter());
    index += frame_float_encode(data + index, voltage);
    index += frame_float_encode(data + index, temp);
    index += frame_float_encode(data + index, el_quantity);
    index += frame_uint8_encode(data + index, state);
    index += frame_float_encode(data + index, sum_time);
    index += frame_float_encode(data + index, work_time);
    index += frame_uint8_encode(data + index, net_state);
    index += frame_uint8_encode(data + index, csq);
    for (int i = 0; i < 30; i++) {
        index += frame_uint8_encode(data + index, 0);
    }
    index += frame_checksum_encode(data, index, data + index);

    return index;
}

uint16_t frame_ip_addr_encode(uint8_t * data, uint8_t * ip)
{
	uint16_t index = 0;
	for(int i = 0; i<4; i++)
	{
		index += frame_uint8_encode(data+index,ip[i]);
	}
	return index;
}

uint16_t frame_netmask_encode(uint8_t * data, uint8_t * netmask)
{
	uint16_t index = 0;
	for(int i = 0; i<4; i++)
	{
		index += frame_uint8_encode(data+index,netmask[i]);
	}
	return index;
}

uint16_t frame_gateway_encode(uint8_t * data, uint8_t * gateway)
{
	uint16_t index = 0;
	for(int i = 0; i<4; i++)
	{
		index += frame_uint8_encode(data+index,gateway[i]);
	}
	return index;
}

uint16_t frame_dns_encode(uint8_t * data, uint8_t * dns)
{
	uint16_t index = 0;
	for(int i = 0; i<4; i++)
	{
		index += frame_uint8_encode(data+index,dns[i]);
	}
	return index;
}

uint16_t frame_net_dev_rsp_encode(uint8_t *data, uint8_t* ip_addr, uint8_t* net_mask, uint8_t*  gateway, uint8_t* dns, uint8_t flag,uint8_t* cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data+index, NET_DEV_CTRL_DATA_LEN, CONTROL_RSP_FRAME,CTRL_DEV_NET_INFO_PKT, cmd_id);
	index += frame_uint8_encode(data+index , flag);
	index += frame_ip_addr_encode(data+index, ip_addr);
	index += frame_netmask_encode(data+index, net_mask);
	index += frame_gateway_encode(data+index, gateway);
	index += frame_dns_encode(data+ index, dns);
	for(int i = 0; i < 16; i++)
	{
		index += frame_uint8_encode(data+index ,SPARE);
	}
	index += frame_checksum_encode(data,index,data+index);

	return index;
}

uint16_t frame_requset_encode(uint8_t * data, uint8_t flag, uint8_t requset_type,uint8_t * cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data+index, REQUSET_RSP_DATA_LEN, CONTROL_RSP_FRAME, SUPERIOR_GET_DATA_PACK_TYPE, cmd_id);
	index += frame_uint8_encode(data+index, flag);
	index += frame_uint8_encode(data+index,requset_type);
	index += frame_checksum_encode(data,index,data+index);

	return index;
}

uint16_t frame_dev_cfg_encode(uint8_t * data, uint8_t flag, uint8_t data_type, uint8_t msg_type, uint8_t * cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data+index, DEV_CFG_SEND_DATA_LEN, CONTROL_RSP_FRAME, CTRL_DEV_CFG_PKT, cmd_id);
	index += frame_uint8_encode(data+index, flag);
	index += frame_uint8_encode(data+index, data_type);
	index += frame_uint8_encode(data+index, msg_type);
	index += frame_checksum_encode(data,index,data+index);

	return index;
}

uint16_t frame_set_or_select_time_rsp_encode(uint8_t * data,uint8_t cmd_statu,  uint32_t timestamp,uint8_t* cmd_id)
{
		uint16_t index = 0;

		index += frame_header_encode(data+index, TIME_CTRL_DATA_LEN, CONTROL_RSP_FRAME, CTRL_TIME_PKT, cmd_id);
	    index += frame_uint8_encode(data+index, cmd_statu);
		index += frame_uint32_encode(data+index,timestamp);
		index += frame_checksum_encode(data,index,data+index);

		return index;
}

uint16_t frame_server_ip_encode(uint8_t *data, uint8_t*ip)
{
	uint16_t index = 0;
	 for(int i = 0; i < 4; i++)
	 {
		 index += frame_uint8_encode(data+index, ip[i]);
	 }
	return index;
}

uint16_t frame_domain_encode(uint8_t *data, uint8_t *damain)
{
	uint16_t index = 0;
	for (int i = 0; i < 63; i++) {
		index += frame_uint8_encode(data + index, damain[i]);
	}
	index += frame_uint8_encode(data + index, '\0');
	for (int i = 0; i < 12; i++) {
		index += frame_uint8_encode(data + index, SPARE);
	}
	return index;
}

uint16_t frame_set_server_info_encode(uint8_t *data, uint8_t flag, uint8_t* ip,uint16_t port, uint8_t* domain ,uint8_t * cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, SERVER_INFO_DATA_LEN,
			CONTROL_RSP_FRAME, CTRL_SERVER_NET_INFO_PKT, cmd_id);
	index += frame_uint8_encode(data + index, flag);
	index += frame_server_ip_encode(data + index, ip);
	index += frame_uint16_encode(data + index, port);
	index += frame_domain_encode(data + index, domain);
	index += frame_checksum_encode(data, index, data + index);

	return index;
}

uint16_t frame_select_server_info_encode(uint8_t *data, uint8_t flag,server_info_t * server_info, uint8_t * cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, SERVER_INFO_DATA_LEN,
			CONTROL_RSP_FRAME, CTRL_SERVER_NET_INFO_PKT, cmd_id);
	index += frame_uint8_encode(data + index, flag);
	index += frame_server_ip_encode(data + index, server_info->ip);
	index += frame_uint16_encode(data + index, server_info->port);
	index += frame_domain_encode(data + index, server_info->domain);
	index += frame_checksum_encode(data, index, data + index);

	return index;
}

uint16_t frame_component_id_encode(uint8_t *data, uint8_t *component_id )
{
	uint16_t index = 0;
	for(int i = 0; i<17;i++){
		index += frame_uint8_encode(data+index,component_id[i]);
	}
	return index;
}

uint16_t frame_component_dev_encode(uint8_t *data, uint8_t flag,uint8_t * component_id,uint16_t original_id, uint8_t * cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, COMPONENT_DEV_DATA_LEN,
			CONTROL_RSP_FRAME, COMPONENT_DEV_PACK_TYPE, cmd_id);
	index += frame_uint8_encode(data + index, flag);
	index += frame_component_id_encode(data + index, component_id);
	index += frame_uint16_encode(data + index, original_id);
	index += frame_checksum_encode(data, index, data + index);

	return index;
}

uint16_t frame_threshold_cfg_encode(uint8_t* data, float threshold, uint8_t type, uint8_t * describe, uint8_t flag, uint8_t sum,uint8_t * cmd_id)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, THRESHOLD_RSP_DATA_LEN + (10 * sum), CONTROL_RSP_FRAME, CTRL_THRESHOLD_PKT, cmd_id);
	index += frame_uint8_encode(data + index, flag);
	index += frame_uint8_encode(data + index, type);
	index += frame_uint8_encode(data + index, sum);
    for (int j = 0; j < sum; j++) {
        for (int i = 0; i < 6; i++) {
            index += frame_uint8_encode(data + index, describe[i]);
        }
        index += frame_float_encode(data + index, threshold);
    }
	index += frame_checksum_encode(data,index,data+index);

	return index;
}

//故障信息报
uint16_t device_fault_msg_encode(uint8_t * data,uint32_t fault_time,uint8_t*  fault_des ,uint16_t des_len, uint8_t *cmd_id)
{
	uint16_t index = 0;
	index += frame_header_encode(data + index, TIMESTAMP_DATA_LEN + des_len,
			WORK_CONDITION_RSP_FRAME, FAULT_MSG_PACK_TYPE,  cmd_id);
	index += frame_uint32_encode(data + index, fault_time);
	for(int i =0; i< des_len; i++)
	{
		index += frame_uint8_encode(data + index, fault_des[i]);
	}
	index += frame_checksum_encode(data, index, data + index);

	return index;
}

uint16_t frame_file_name_encode(uint8_t * data, uint8_t * file_name)
{
	uint16_t index = 0;
	for (int i = 0; i < 20; i++) {
		index += frame_uint8_encode(data, file_name[i]);
	}
	return index;
}

uint16_t frame_miss_pack_list_encode(uint8_t * data, uint32_t * miss_list,uint16_t miss_count)
{
	uint16_t index = 0;
	for (int i = 0; i < miss_count; i++) {
		index += frame_uint32_encode(data+index, miss_list[i]);
	}
	return index;
}

uint16_t frame_miss_pack_info_encode(uint8_t *data, uint8_t *cmd_id, uint8_t *file_name)
{
    uint16_t index = 0;
    uint32_t miss_count = 0;
    uint32_t miss_idx[110] = {0};
    recv_pack_t recv = get_recv_pack_info();

		for (uint32_t  i = 0; i < recv.file_count; i++) {
			if (!bf_isset(recv.bit_filed, i)) {
				if(miss_count <= 110){
				miss_idx[miss_count++] = i + 1;
				}
			}
		}
    index += frame_header_encode(data+index, UPGRADE_MISS_PACK_DATA_LEN + 4 * miss_count, CONTROL_RSP_FRAME, CTRL_UPGRADE_MISS_FILE_PKT, cmd_id);
    index += frame_file_name_encode(data+index, file_name);
    index += frame_uint32_encode(data+index, miss_count);
    index += frame_miss_pack_list_encode(data+index, miss_idx, miss_count);
    index += frame_checksum_encode(data, index, data + index);

    return index;
}

uint16_t frame_out_sheath_cable_monitor_encode(uint8_t * data,mx_sensor_info_t* p,uint8_t packet_type)
{
    uint16_t index = 0;
	 uint8_t datalen = 0x2b;

	 if(NULL == p)
	 {
		 return 1;
	 }
	 index += frame_header_encode(data+index,datalen,MONITOR_DATA_FRAME,packet_type,p->cmd_id);
	 for(int i = 0; i<17;i++)
	 {
	  	index += frame_uint8_encode(data+index, p->component_id [i]);
	 }
	 uint16_t alerm_flag = 0;
	 index += frame_uint32_encode(data+index,p->timestamp);
	 index += frame_uint16_encode(data+index,alerm_flag);
	 index += frame_float_encode(data+index,p->max);
	 index += frame_float_encode(data+index,p->min);
	 index += frame_float_encode(data+index,p->realtime);
	 index += frame_uint32_encode(data+index,0);
	 index += frame_uint32_encode(data+index,0);
	 index += frame_checksum_encode(data,index,data+index);

    return index;
}

uint16_t frame_dev_cur_encode(uint8_t * data,mx_sensor_info_t* p)
{
	 if(NULL == p)
	 {
		 return 1;
	 }
	 uint8_t   packet_type = CUR_DATA_UPLOAD_PKT;
	 return  frame_out_sheath_cable_monitor_encode(data,p,packet_type);
}

uint16_t frame_dev_alarm_encode(uint8_t * data, st_sensor_info_t *p)
{
    uint16_t index = 0;
    uint8_t datalen = 0x21;

    index += frame_header_encode(data + index, datalen, MONITOR_DATA_FRAME, ALARM_STATUS_UPLOAD_PKT, p->cmd_id);
    for (int i = 0; i < 17; i++) {
        index += frame_uint8_encode(data + index, p->component_id[i]);
    }
    index += frame_uint32_encode(data + index, p->timestamp);
    index += frame_uint16_encode(data + index, p->defense);
    index += frame_uint16_encode(data + index, p->status);
    index += frame_uint32_encode(data + index, 0);
    index += frame_uint32_encode(data + index, 0);
    index += frame_checksum_encode(data, index, data + index);

    return index;
}

uint16_t frame_dev_sc_alarm_encode(uint8_t * data, sc_sensor_info_t *p)
{
    uint16_t index = 0;
    uint8_t datalen = 0x23;

    index += frame_header_encode(data + index, datalen, MONITOR_DATA_FRAME, SHORT_CIRCUIT_ALARM_UPLOAD_PKT, p->cmd_id);
    for (int i = 0; i < 17; i++) {
        index += frame_uint8_encode(data + index, p->component_id[i]);
    }
    index += frame_uint32_encode(data + index, p->timestamp);
    index += frame_uint16_encode(data + index, p->exist_flag);  //alarm
    index += frame_uint16_encode(data + index, p->current);
    index += frame_uint8_encode(data + index, !!p->alarm);
    index += frame_uint8_encode(data + index, 0);  //direction change
    index += frame_uint32_encode(data + index, 0);
    index += frame_uint32_encode(data + index, 0);
    index += frame_checksum_encode(data, index, data + index);

    return index;
}


uint16_t frame_sample_ctrl_rsp_encode (uint8_t * data,uint8_t command_status,sample_param_t* p_sample, uint8_t * cmd_id)
{
	 uint16_t index =0;
	 uint8_t datalen = 12;
	 sample_param_t *p = p_sample ;
	 index += frame_header_encode(data+index,datalen,CONTROL_RSP_FRAME,CTRL_SAMPLE_INFO_PKT, cmd_id);
	index += frame_uint8_encode(data+index,command_status);
	index += frame_uint8_encode(data+index,p->request_type);
	index += frame_uint16_encode(data+index,p->main_time);
	index += frame_uint16_encode(data+index,p->sample_count);
	index += frame_uint16_encode(data+index,p->sample_frequency);
	index += frame_uint32_encode(data+index,p->reserve);
	index += frame_checksum_encode(data,index,data+index);
	 return index;
}

uint16_t frame_dev_reset_rsp_encode(uint8_t* data, uint8_t command_status, uint8_t * cmd_id)
{
	 uint16_t index =0;

	 index += frame_header_encode(data+index,sizeof(uint8_t),CONTROL_RSP_FRAME,CTRL_DEV_RESET_PKT, cmd_id);
	index += frame_uint8_encode(data+index,command_status);
	index += frame_checksum_encode(data,index,data+index);
	return index;
}

uint16_t frame_dev_ip_addr_encode(uint8_t* data, uint8_t flag, uint8_t * p_num, uint8_t *ip_addr,uint8_t * cmd_id )
{
	uint16_t index = 0;

	index += frame_header_encode(data+index, GET_DEV_IP_DATA_LEN, CONTROL_RSP_FRAME, CTRL_DEV_IP_PACK_TYPE, cmd_id);
	index += frame_uint8_encode(data+index, flag);
	for(int i =0; i<16; i++)
	{
		index += frame_uint8_encode(data+index, p_num[i]);
	}
	for(int i =0; i<4; i++)
	{
		index += frame_uint8_encode(data+index, ip_addr[i]);
	}
	index += frame_uint32_encode(data+index, SPARE);
	index += frame_checksum_encode(data,index,data+index);

	return index;
}

uint16_t frame_dev_vol_encode(uint8_t * data, mx_sensor_info_t* p)
{
    if (NULL == p) {
        return 1;
    }
    return frame_out_sheath_cable_monitor_encode(data, p, VOL_DATA_UPLOAD_PKT);
}

uint16_t frame_dev_hfct_encode(uint8_t * data, mx_sensor_info_t* p)
{
    uint16_t index = 0;
    uint8_t datalen = 0x25;
    if (NULL == p) {
        return 1;
    }
    index += frame_header_encode(data + index, datalen, MONITOR_DATA_FRAME, HFCT_DATA_UPLOAD_PKT, p->cmd_id);
    for (int i = 0; i < 17; i++) {
        index += frame_uint8_encode(data + index, p->component_id[i]);
    }
    index += frame_uint32_encode(data + index, p->timestamp);
    index += frame_uint16_encode(data + index, 0);
    index += frame_float_encode(data + index,p->max);
    index += frame_uint16_encode(data + index, p->freq ); //FIXME: set valid value
    index += frame_uint32_encode(data + index, 0);
    index += frame_uint32_encode(data + index, 0);
    index += frame_checksum_encode(data, index, data + index);

    return index;
}

uint16_t frame_temperature_encode(uint8_t *data, mx_sensor_info_t* p)
{
    if (NULL == p) {
        return 1;
    }
    return frame_out_sheath_cable_monitor_encode(data, p, TEMP_DATA_UPLOAD_PKT);
}
