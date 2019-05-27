#include "frame_handler.h"

#include <string.h>

#include "periph/rtt.h"

#include "upgrade_from_flash.h"
#include "log.h"
//#include "daq.h"
#include "frame_encode.h"
#include "frame_common.h"
#include "periph/rtt.h"
#include "ec20_at.h"
#include "periph/pm.h"
#include "history_data_send.h"
#include "app_upgrade.h"
#include "type_alias.h"
//#include "temp_bat_vol.h"
#include "heart_beat.h"
#include "data_transfer.h"
#include "vc_temp_bat_vol_sample.h"
#include "dev_cfg.h"
#include "data_processor.h"
#include "timex.h"
#include "xtimer.h"
//extern void set_interval_second(uint32_t seconds);

enum {
    READ = 0,
    WRITE
} ctrl_req_flag_t;

static kernel_pid_t history_data_send_pid;

static void delay_s(int s)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
}

static void delay_ms(int ms)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}

void history_data_send_pid_hook(kernel_pid_t pid)
{
	history_data_send_pid = pid;
}

void msg_handler_packet_send(uint8_t *data, uint32_t data_len)
{
	if(get_ec20_link_flag() == LINK_UP){
		while(set_data_to_cache(data, data_len) == 0){
			delay_ms(500);
		}
	}
}

void ctrl_timestamp_handler(frame_req_t *frame_req)
{
    uint16_t length;
    uint8_t data[MAX_REQ_FRAME_LEN];
    uint32_t timestamp = 0;

    if (frame_req->frame_req.device_time.type == 1) {
        timestamp = frame_req->frame_req.device_time.time;
        rtt_set_counter(timestamp);
    }
    else {
        timestamp = rtt_get_counter();
    }

    length = frame_set_or_select_time_rsp_encode(data, 0xFF, timestamp, frame_req->header.cmd_id);

    LOG_INFO("Send time command success");
    msg_handler_packet_send(data, length);
    return;
}

void do_set_cmd_id(uint8_t *cmd_id, component_dev_t * com_dev)
{
    st_sensor_info_t* st = get_dev_alarm_info();
    mx_sensor_info_t mx;

//    switch (com_dev->req_type) {
//    case TEMP_CTRL_REQ_TYPE:
//        if (SD_TRUE == get_dev_temp_info_by_cmd_id(cmd_id, &mx)) {
//            memcpy(mx.cmd_id, com_dev->id, ID_LEN);
//            update_dev_temp_info(mx.phase, &mx);
//        }
//        break;
//    case CURRENT_CTRL_REQ_TYPE:
        if (SD_TRUE == get_dev_cur_info_by_cmd_id(cmd_id, &mx)) {
            memcpy(mx.cmd_id, com_dev->id, ID_LEN);
            update_dev_cur_info(mx.phase, &mx);
        }
        mx_sensor_info_t htfc;
//        break;
//    case HFCT_CTRL_REQ_TYPE:
        if (SD_TRUE == get_dev_hfct_info_by_cmd_id(cmd_id, &htfc)) {
            memcpy(htfc.cmd_id, com_dev->id, ID_LEN);
            update_dev_hfct_info(htfc.phase, &htfc);
        }
//        break;
//    case VOLTAGE_CTRL_REQ_TYPE:
        mx_sensor_info_t vol;
        if (SD_TRUE == get_dev_vol_info_by_cmd_id(cmd_id, &vol)) {
            memcpy(vol.cmd_id, com_dev->id, ID_LEN);
            update_dev_vol_info(vol.phase, &vol);
        }
//        break;
//    case ALARM_CTRL_REQ_TYPE:
        sc_sensor_info_t sl;

        if (SD_TRUE == get_dev_sc_alarm_info_by_cmd_id(cmd_id, &sl)) {
            memcpy(sl.cmd_id, com_dev->id, ID_LEN);
            update_dev_sc_alarm_info(sl.phase, &sl);
        }

        if (0 == strncmp((char*)st->cmd_id, (char*)cmd_id, ID_LEN)) {
            memcpy(st->cmd_id, com_dev->id, ID_LEN);
            update_dev_alarm_info(st);
        }
//        break;
//    default:
//        LOG_ERROR("Error request sensor type");
//        break;
//    }
}

void do_set_component_id(uint8_t *cmd_id, component_dev_t * com_dev)
{
    st_sensor_info_t* st = get_dev_alarm_info();
    mx_sensor_info_t mx;

//    switch (com_dev->req_type) {
//    case TEMP_CTRL_REQ_TYPE:
        if (SD_TRUE == get_dev_temp_info_by_cmd_id(cmd_id, &mx)) {
            memcpy(mx.component_id, com_dev->id, ID_LEN);
            update_dev_temp_info(mx.phase, &mx);
        }
//        break;
//    case CURRENT_CTRL_REQ_TYPE:
        if (SD_TRUE == get_dev_cur_info_by_cmd_id(cmd_id, &mx)) {
            memcpy(mx.component_id, com_dev->id, ID_LEN);
            update_dev_cur_info(mx.phase, &mx);
        }
//        break;
//    case HFCT_CTRL_REQ_TYPE:
        if (SD_TRUE == get_dev_hfct_info_by_cmd_id(cmd_id, &mx)) {
            memcpy(mx.component_id, com_dev->id, ID_LEN);
            update_dev_hfct_info(mx.phase, &mx);
        }
//        break;
//    case VOLTAGE_CTRL_REQ_TYPE:
        if (SD_TRUE == get_dev_vol_info_by_cmd_id(cmd_id, &mx)) {
            memcpy(mx.component_id, com_dev->id, ID_LEN);
            update_dev_vol_info(mx.phase, &mx);
        }
//        break;
//    case ALARM_CTRL_REQ_TYPE:
        if (0 == strncmp((char*)st->component_id, (char*)cmd_id, ID_LEN)) {
            memcpy(st->component_id, com_dev->id, ID_LEN);
            update_dev_alarm_info(st);
        }
//        break;
//    default:
//        LOG_ERROR("Error request sensor  type");
//        break;
//    }
}

int get_componet_id(uint8_t * compoent_id, frame_req_t *frame_req)
{
	for(int i = 0; i<3; i++)
	{
		mx_sensor_info_t *mx = get_dev_vol_info_by_phase(i);
		if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
		{
			strncpy((char *)compoent_id,(char *)mx->cmd_id, 17);
			return 1;
		}
	}
	for(int i = 0; i<3; i++)
	{
		mx_sensor_info_t *mx = get_dev_cur_info_by_phase(i);
		if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
		{
			strncpy((char *)compoent_id,(char *)mx->cmd_id, 17);
			return 1;
		}
	}
	for(int i = 0; i<3; i++)
	{
		mx_sensor_info_t *mx = get_dev_hfct_info_by_phase(i);
		if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
		{
			strncpy((char *)compoent_id,(char *)mx->cmd_id, 17);
			return 1;
		}
	}
	for(int i = 0; i<3; i++)
	{
		sc_sensor_info_t* mx = get_dev_sc_alarm_info_by_phase(i);
		if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
		{
			strncpy((char *)compoent_id,(char *)mx->cmd_id, 17);
			return 1;
		}
	}
	st_sensor_info_t*mx = get_dev_alarm_info();
	if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
	{
		strncpy((char *)compoent_id,(char *)mx->cmd_id, 17);
		return 1;
	}
	return 0;
}


void get_component_dev_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    uint8_t component_id[ID_LEN] = {0};

    if (get_componet_id(component_id,frame_req)) {
        length = frame_component_dev_encode(data, SET_SUCCESS, component_id, frame_req->frame_req.component_dev.original_id, frame_req->header.cmd_id);
    }
    else {
        length = frame_component_dev_encode(data, SET_ERROR, component_id, frame_req->frame_req.component_dev.original_id, frame_req->header.cmd_id);
    }
    LOG_INFO(":get_component_dev_handler rsp");
    msg_handler_packet_send(data, length);
}

void set_cmd_id_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    component_dev_t *com_dev = &frame_req->frame_req.component_dev;

    do_set_cmd_id(frame_req->header.cmd_id, com_dev);
    length = frame_component_dev_encode(data, SET_SUCCESS, com_dev->id, frame_req->frame_req.component_dev.original_id, frame_req->header.cmd_id);
    LOG_INFO("Set_cmd_id_handler rsp");
    msg_handler_packet_send(data, length);
}

void set_component_dev_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    component_dev_t *com_dev = &frame_req->frame_req.component_dev;

    do_set_component_id(frame_req->header.cmd_id, com_dev);
    length = frame_component_dev_encode(data, SET_SUCCESS, com_dev->id, frame_req->frame_req.component_dev.original_id, frame_req->header.cmd_id);
    LOG_INFO("Set component dev handler rsp");
    msg_handler_packet_send(data, length);
}

void ctrl_dev_net_handler(frame_req_t *frame_req)
{
    uint8_t result;
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    net_info_t* self = get_dev_net_info();

    if (frame_req->frame_req.net_dev.type == READ) {//FIXME: get self network info from 4G with AT command
    	get_domain_name((char *)self->ip);
        result = SET_SUCCESS;
    }
    else {
        result = SET_ERROR;
    }

    length = frame_net_dev_rsp_encode(data, self->ip, self->netmask, self->gateway, self->dns, result, frame_req->header.cmd_id);
    LOG_INFO("Ctrl dev net handler rsp");
    msg_handler_packet_send(data, length);

    return;
}

void start_send_history_data( request_data_t *requset)
{
	msg_t msg;
	msg.type = HIS_DATA;
	set_history_time_interval( requset->start_time, requset->end_time, requset->request_type);
	msg_send(&msg,history_data_send_pid);
}

int check_id_with_true_data(frame_req_t *frame_req)
{
		if((frame_req->frame_req.request_data.request_type ==CUR_DATA_UPLOAD_PKT))
		{
			for(int i = 0; i<3; i++)
			{
				mx_sensor_info_t *mx = get_dev_cur_info_by_phase(i);
				if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
				{
					return 1;
				}
			}
		}
		if((frame_req->frame_req.request_data.request_type ==HFCT_DATA_UPLOAD_PKT))
		{
			for(int i = 0; i<3; i++)
			{
				mx_sensor_info_t *mx = get_dev_hfct_info_by_phase(i);
				if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
				{
					return 1;
				}
			}
		}
		if((frame_req->frame_req.request_data.request_type ==VOL_DATA_UPLOAD_PKT))
		{
			for(int i = 0; i<3; i++)
			{
				mx_sensor_info_t *mx = get_dev_vol_info_by_phase(i);
				if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
				{
					return 1;
				}
			}
		}
		if((frame_req->frame_req.request_data.request_type ==SHORT_CIRCUIT_ALARM_UPLOAD_PKT))
		{
			for(int i = 0; i<3; i++)
			{
				sc_sensor_info_t* mx =get_dev_sc_alarm_info_by_phase( i);
				if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
				{
					return 1;
				}
			}
		}
		if((frame_req->frame_req.request_data.request_type ==ALARM_STATUS_UPLOAD_PKT))
		{
			st_sensor_info_t* mx =get_dev_alarm_info();
			if(strncmp((char *)mx->cmd_id, (char *)frame_req->header.cmd_id, 17) == 0)
			{
				return 1;
			}
		}
		return 0;
}


void superior_request_data_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    request_data_t *requset = &frame_req->frame_req.request_data;
    //TODO:响应最后一条
    if(check_id_with_true_data(frame_req)){
    start_send_history_data(requset);
    length = frame_requset_encode(data, SET_SUCCESS, requset->request_type, frame_req->header.cmd_id);
    LOG_INFO("Superior request data handler rsp");
    msg_handler_packet_send(data, length);
    }
}

void ctrl_sample_info_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    uint8_t status = SET_SUCCESS;

    sample_param_t *p = &frame_req->frame_req.sample_param;

    mx_sensor_info_t *mx = NULL;

	mx = get_dev_cur_info_by_phase(0);

    if (READ == p->request_set_flag) {
            p->main_time = mx->interval;
    }
    else if (WRITE == p->request_set_flag) {
            mx->interval = p->main_time;
            update_dev_cur_info(0, mx);
    }

    length = frame_sample_ctrl_rsp_encode(data, status, p, frame_req->header.cmd_id);
    LOG_INFO("Ctrl sample info handler rsp");
    msg_handler_packet_send(data, length);
}

void ctrl_threshold_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

    threshold_t *threshold = &frame_req->frame_req.threshold_cfg;
    sensor_t type = ALARM;
    uint8_t status = SET_ERROR;
    boolean ret = SD_FALSE;

    mx_sensor_info_t* mx = NULL;
    st_sensor_info_t* st = NULL;
    sc_sensor_info_t* sc = NULL;

    switch (threshold->req_type) {
    case TEMP_CTRL_REQ_TYPE:
        type = TEMP;
        ret = get_dev_temp_info_by_cmd_id(frame_req->header.cmd_id, mx);
        break;
    case CURRENT_CTRL_REQ_TYPE:
        type = CUR;
        ret =  get_dev_temp_info_by_cmd_id(frame_req->header.cmd_id, mx);
        break;
    case HFCT_CTRL_REQ_TYPE:
        type = HFCT;
        ret = get_dev_hfct_info_by_cmd_id(frame_req->header.cmd_id, mx);
        break;
    case VOLTAGE_CTRL_REQ_TYPE:
        type = VOL;
        ret = get_dev_vol_info_by_cmd_id(frame_req->header.cmd_id, mx);
        break;
    case ALARM_CTRL_REQ_TYPE:
        type = ALARM;
        st = get_dev_alarm_info();
        break;
    case SC_ALARM_CTRL_REQ_TYPE:
        type = SC_ALARM;
        ret  =  get_dev_sc_alarm_info_by_cmd_id(frame_req->header.cmd_id, sc);
        break;
    default:
        status = SET_ERROR;
        LOG_INFO("Invalid ctrl request type for threshold command.");
        break;
    }

    if (READ == threshold->data_type) {
        status = SET_SUCCESS;
        if (ALARM_CTRL_REQ_TYPE == threshold->req_type) {
            threshold->thr_data[0].threshold = (uint32_t)st->threshold;
        }
        else if (SC_ALARM_CTRL_REQ_TYPE == threshold->req_type) {
            threshold->thr_data[0].threshold = (uint32_t)sc->threshold;
        }
        else {
            threshold->thr_data[0].threshold = (uint32_t)mx->threshold;
        }
    }
    else if (threshold->sum != 1) {     //cg-ibox only 1 threshold can be set (single sensor)
        status = SET_ERROR;
    }
    else if (WRITE == threshold->data_type) {
        status = SET_SUCCESS;
        if (ALARM_CTRL_REQ_TYPE == threshold->req_type) {
            st->threshold = threshold->thr_data[0].threshold;
            update_dev_alarm_info(st);
        }
        else if (SC_ALARM_CTRL_REQ_TYPE == threshold->req_type) {
        	if(ret == SD_TRUE){
            sc->threshold = threshold->thr_data[0].threshold;
            update_dev_sc_alarm_info(sc->phase, sc);
        	}
        }
        else {
        	if(ret == SD_TRUE){
            mx->threshold = threshold->thr_data[0].threshold;
            update_dev_mx_info_by_type_and_phase(type, mx->phase, mx);
        	}
        }
    }

    length = frame_threshold_cfg_encode(data, mx->threshold, threshold->req_type, threshold->thr_data[0].warn_cfg, status, 1, frame_req->header.cmd_id);
    LOG_INFO("Ctrl threshold handler");
    msg_handler_packet_send(data, length);
    return;
}

void reconnect_to_server(void)
{
    server_info_t* server = get_dev_server_info();

    //FIXME: reconnect to server with 4G
    LOG_INFO("IP adddr %d.%d.%d.%d\r\n",server->ip[0],server->ip[1],server->ip[2],server->ip[3]);
    set_server_ip_port((char *)server->ip, server->port);
}

void ctrl_server_net_info_hadler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    server_cfg_select_t *server_info = &frame_req->frame_req.server_cfg;
    uint8_t status = SET_ERROR;
    server_info_t* server = get_dev_server_info();

    if (WRITE == frame_req->frame_req.server_cfg.req_type) {
        //TODO: check IP format first
        memcpy(server->ip, server_info->ip, 4);
        server->port = server_info->port;
        memcpy(server->domain, server_info->domain, 64);
        status = SET_SUCCESS;
        update_dev_server_info(server);
        reconnect_to_server();
    }
    else if (READ == frame_req->frame_req.server_cfg.req_type) {
        status = SET_SUCCESS;
    }

    length = frame_set_server_info_encode(data, status, server->ip, server->port, server->domain, frame_req->header.cmd_id);
    LOG_INFO("Ctrl server net info hadler rsp");
    msg_handler_packet_send(data, length);
}

void component_dev_ctrl_handler(frame_req_t *frame_req)
{
    uint8_t type = frame_req->frame_req.component_dev.req_type;
    switch (type) {
    case 0:
        get_component_dev_handler(frame_req);
        break;
    case 1:
        set_component_dev_handler(frame_req);
        break;
    case 2:
        set_cmd_id_handler(frame_req);
        break;
    default:
    	LOG_INFO("Invalid ops type for CTRL_ID_PKT.");
        break;
    }
}


void ctrl_dev_cfg_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    dev_cfg_select_t *dev_cfg = &frame_req->frame_req.dev_cfg_select;
    product_info_t* product = get_dev_product_info();
    uint8_t status = SET_ERROR;

    //FIXME: get real data for follow data
    float voltage = (float)get_bat_vol();
    float temp = (float)get_line_temp(1);
    int csq = get_ec20_csq_rssi();

    uint8_t charge = 0;
    uint8_t net_state = 0;

    if (WRITE == dev_cfg->req_type) {
        status = SET_ERROR;
    }
    else {
        status = SET_SUCCESS;
    }

    length = frame_dev_cfg_encode(data, status, dev_cfg->data_type, dev_cfg->msg_type, frame_req->header.cmd_id);
    LOG_INFO("1.ctrl dev cfg handler rsp");
    msg_handler_packet_send(data, length);

    delay_s(2);
    memset(data, 0, sizeof(data));

    if (1 == dev_cfg->msg_type) {//8.2
        length = frame_device_config_rsp_encode(data, product, frame_req->header.cmd_id);
        LOG_INFO("2.ctrl dev cfg handler rsp");
        msg_handler_packet_send(data, length);
    }
    else if (2 == dev_cfg->msg_type) {//8.3
        length = frame_work_condition_rsp_encode(data, frame_req->header.cmd_id, voltage, temp, voltage / 12.6, charge, get_total_runtime(), get_current_runtime(), net_state, csq);
        LOG_INFO("3.ctrl dev cfg handler rsp");
        msg_handler_packet_send(data, length);
    }
}

void do_send_upgrade_file_error_msg(uint8_t *cmd_id, uint8_t *file_name)
{
	uint16_t length = 0;
	uint8_t data[512] = { 0 };

	length = frame_miss_pack_info_encode(data, cmd_id, file_name);
	LOG_INFO("Send upgrade file error msg rsp\r\n");
	msg_handler_packet_send(data, length);
}

void app_upgrade_cnf_handler(frame_req_t *frame_req)
{
    if (SD_TRUE == check_file_info_integrity(frame_req->frame_req.transfer_file_req.file_count)) {
    	clear_recv_file_info();
    }
    else {
        do_send_upgrade_file_error_msg(frame_req->header.cmd_id, frame_req->frame_req.transfer_file_req.file_name);
    }
}

void app_upgrade_handler(frame_req_t *frame_req)
{
    uint32_t file_count = 0;
    uint32_t file_index = 0;

    uint8_t last_packet = 1;
    static int  first = 1;

    file_count = frame_req->frame_req.transfer_file_req.file_count;
    file_index = frame_req->frame_req.transfer_file_req.file_index -1;

    if (first == 1) {
        clear_recv_file_info();
        LOG_INFO("Start to upload arm program file, during file transfer, ignore new data.\r\n");
        set_arm_file_transfer_flag(SD_TRUE);
        first = 0;
    }
    add_recv_pkt_info(file_count, file_index);

    if (file_index >= file_count) {
    	LOG_INFO("Upload file request frame invalid: file_index(%ld) >= file_count(%ld)\r\n", file_index, file_count);
        set_arm_file_transfer_flag(SD_FALSE);
    }
    else if (SD_TRUE != process_upload_file_req(&frame_req->frame_req.transfer_file_req, file_index, &last_packet)) {
    	LOG_INFO("process upgrade file request failed!, file_index(%ld), file_count(%ld)\r\n", file_index, file_count);
        set_arm_file_transfer_flag(SD_FALSE);
    }
    LOG_INFO("Get upgrade file pkt info: total:%ld, index:%ld, last_flag:%d\r\n", file_count, file_index, last_packet);

    if (SD_TRUE == check_file_info_integrity(file_count)) {//we can't recognize message first transfer or refransfer, so check every time
    	do_send_upgrade_file_error_msg(frame_req->header.cmd_id, frame_req->frame_req.transfer_file_req.file_name);
    	process_upgrade_programe_req(ARM);
        clear_recv_file_info();
    }
}

int check_register_all_success(register_info_t *register_info)
{
	if(register_info->error_count != 0)
	{
		return -1;
	}
	for(int i = 0; i< 3; i++)
	{
		if(register_info->cmd_id[i] != 0)
		{
			return -1;
		}
	}
	for(int i = 3; i< 6; i++)
	{
		if(register_info->cmd_id[i] != 0)
		{
			return -1;
		}
	}
//	for(int i = 6; i< 9; i++)
//	{
//		if(register_info->cmd_id[i] != 0)
//		{
//			return -1;
//		}
//	}
	for(int i = 9; i< 12; i++)
	{
		if(register_info->cmd_id[i] != 0)
		{
			return -1;
		}
	}
	for(int i = 12; i< 15; i++)
	{
		if(register_info->cmd_id[i] != 0)
		{
			return -1;
		}
	}
	if(register_info->cmd_id[15] != 0)
	{
		return -1;
	}

	return 0;
}

void flash_register_error_info_by_cmd_id(uint8_t * cmd_id, dev_register_t * dev_register)
{
	mx_sensor_info_t* mx_register_info = NULL;
	st_sensor_info_t* st_register_info = NULL;
	sc_sensor_info_t* sc_register_info = NULL;
	register_info_t *register_info = get_register_info();

	for (int i = 0; i < 3; i++) {
		mx_register_info = get_dev_cur_info_by_phase(i);
		if ((strncmp((const char *)cmd_id, (const char *)mx_register_info->cmd_id, 17) == 0) && (dev_register->register_flag == 0 )) {
			register_info->error_count--;
			register_info->cmd_id[i] = 0;
			LOG_INFO("register  %d  cur info ok!", i+1);
		}
	}
	for (int i = 0; i < 3; i++) {
		mx_register_info= get_dev_vol_info_by_phase(i);
		if ((strncmp((const char *)cmd_id, (const char *)mx_register_info->cmd_id, 17) == 0 ) && (dev_register->register_flag == 0 )) {
			register_info->error_count--;
			register_info->cmd_id[i + 3] = 0;
			LOG_INFO("register  %d vol info ok!", i+1);
		}
	}
//	for (int i = 0; i < 3; i++) {
//		mx_register_info = get_dev_temp_info_by_phase(i);
//		if ((strncmp((const char *)cmd_id, (const char *)mx_register_info->cmd_id, 17) == 0) && (dev_register->register_flag == 0 )) {
//			register_info->error_count--;
//			register_info->cmd_id[i + 6] = 0;
//			LOG_INFO("register  %d temp info ok!\r\n", i+1);
//		}
//	}
	for (int i = 0; i < 3; i++) {
		mx_register_info = get_dev_hfct_info_by_phase(i);
		if ((strncmp((const char *)cmd_id, (const char *)mx_register_info->cmd_id, 17) == 0) && (dev_register->register_flag == 0 )) {
			register_info->error_count--;
			register_info->cmd_id[i + 9] = 0;
			LOG_INFO("register  %d hfct info ok!", i+1);
		}
	}
	for (int i = 0; i < 3; i++) {
		sc_register_info = get_dev_sc_alarm_info_by_phase(i);
		if ((strncmp((const char *)cmd_id, (const char *)sc_register_info->cmd_id, 17) == 0) && (dev_register->register_flag == 0 )) {
			register_info->error_count--;
			register_info->cmd_id[i + 12] = 0;
			LOG_INFO("register %d sc alarm info ok!", i+1);
		}
	}
	st_register_info = get_dev_alarm_info();
	if ((strncmp((const char *)cmd_id, (const char *)st_register_info->cmd_id, 17) == 0) && (dev_register->register_flag == 0 )) {
		register_info->error_count--;
		register_info->cmd_id[15] = 0;
		LOG_INFO("register alarm info ok!");
	}
	if (check_register_all_success(register_info) == 0 ) {
		register_info->all_success_flag = 1;
		LOG_INFO("register all success ok!");
		bl_time_t*bl_time =  get_bl_time();
		if(bl_time->break_time != 0){
		msg_t msg;
		bl_time->linked_time = rtt_get_counter();
		msg.type = DSC_DATA;
		msg_send(&msg, history_data_send_pid);
		}
	}
}

void dev_config_frame_handler(frame_req_t *frame_req)
{
	flash_register_error_info_by_cmd_id(frame_req->header.cmd_id, &frame_req->frame_req.dev_register);
}

void work_status_frame_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    float voltage = (float)get_bat_vol();
    float current_time = get_current_runtime();
    float total_time = get_total_runtime();
    float temp = get_line_temp(0);
    int csq = get_ec20_csq_rssi();

    length = frame_work_condition_rsp_encode(data, frame_req->header.cmd_id, voltage, temp, voltage / 12.6, 0x00, total_time,
                                             current_time, 0x00, csq);
    LOG_INFO("work status frame handler rsp...");
    msg_handler_packet_send(data, length);
}

void fault_msg_frame_handler(frame_req_t * frame_req){
	(void) frame_req;
}

void ctrl_device_reset_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    uint16_t reset_mode = frame_req->frame_req.dw_commstatus;

    length = frame_dev_reset_rsp_encode(data, SET_SUCCESS, frame_req->header.cmd_id);
    LOG_INFO("ctrl_device_reset_handler rsp...");
    msg_handler_packet_send(data, length);
    delay_s(2);

    if (0x00 == reset_mode) {
        soft_reset();
    }
    else {
    	LOG_INFO("Unsupported device reset model\r\n");
    }
}

void ctrl_dev_ip(frame_req_t *frame_req)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	uint8_t p_num[16] = { 0 };
	net_info_t* self =  get_dev_net_info();
	uint16_t p_num_len = get_ec20_imei((char *)p_num);
	if(p_num_len  != 16)
	{
		p_num[15] = '\0';
	}

	if (frame_req->frame_req.dev_ip.req_type == 0x01) {
		length = frame_dev_ip_addr_encode(data, SET_ERROR,
				frame_req->frame_req.dev_ip.no, self->ip, frame_req->header.cmd_id);
		LOG_INFO("1.ctrl dev ip rsp");
		msg_handler_packet_send(data, length);
	} else {
		length = frame_dev_ip_addr_encode(data, SET_SUCCESS, p_num, self->ip,
				frame_req->header.cmd_id);
		LOG_INFO("2.ctrl dev ip rsp");
		msg_handler_packet_send(data, length);
	}
}

void work_condition_frame_handler(frame_req_t *frame_req){
	switch (frame_req->msg_type){
	case DEV_CONFIG_PACK_TYPE :
		LOG_INFO("Receive dev config command\r\n");
		dev_config_frame_handler(frame_req);
		break;
	case WORK_CONDITION_PACK_TYPE:
		work_status_frame_handler(frame_req);
		break;
	case FAULT_MSG_PACK_TYPE:
//		fault_msg_frame_handler(frame_req);
		break;
	default:
		LOG_INFO("Get invalid message type: %02XH, please check again.\r\n", frame_req->msg_type);
		break;
	}
	return;
}

void frame_check_send_data(frame_req_t *master_frame_req)
{
	uint8_t *flag = 	get_data_send_flag();
	st_sensor_info_t* mx =get_dev_alarm_info();
	if((strncmp((char *)mx->cmd_id, (char *)master_frame_req->header.cmd_id, 17) == 0))
	{
		flag[0] = 1;
	}
	for(int i = 0; i<3; i++)
	{
		mx_sensor_info_t *mx = get_dev_vol_info_by_phase(i);
		if(strncmp((char *)mx->cmd_id, (char *)master_frame_req->header.cmd_id, 17) == 0)
		{
			flag[i+1] = 1;
		}
	}
	for(int i = 0; i<3; i++)
	{
		sc_sensor_info_t* mx =get_dev_sc_alarm_info_by_phase( i);
		if(strncmp((char *)mx->cmd_id, (char *)master_frame_req->header.cmd_id, 17) == 0)
		{
			flag[i+4] = 1;
		}
	}
	for(int i = 0; i<3; i++)
	{
		mx_sensor_info_t *mx = get_dev_cur_info_by_phase(i);
		if(strncmp((char *)mx->cmd_id, (char *)master_frame_req->header.cmd_id, 17) == 0  )
		{
			flag[i+7] = 1;
		}
	}
	for(int i = 0; i<3; i++)
		{
			mx_sensor_info_t *mx = get_dev_hfct_info_by_phase(i);
			if(strncmp((char *)mx->cmd_id, (char *)master_frame_req->header.cmd_id, 17) == 0 )
			{
				flag[i+10] = 1;
			}
		}
}

//#define MAX_UPLOAD_DATA_RETRY_TIMES (5)
void do_monitor_rsp_handler(frame_req_t *frame_req)
{
    uint8_t data_status = frame_req->frame_req.w_commstatus;

    if (SET_SUCCESS == data_status) {
        switch (frame_req->msg_type) {
        case CUR_DATA_UPLOAD_PKT:
        	frame_check_send_data(frame_req);
            break;
        case VOL_DATA_UPLOAD_PKT:
        	frame_check_send_data(frame_req);
            break;
        case HFCT_DATA_UPLOAD_PKT:
        	frame_check_send_data(frame_req);
            break;
        case TEMP_DATA_UPLOAD_PKT:
        	frame_check_send_data(frame_req);
            break;
        case ALARM_STATUS_UPLOAD_PKT:
        	frame_check_send_data(frame_req);
            break;
        case SHORT_CIRCUIT_ALARM_UPLOAD_PKT:
        	frame_check_send_data(frame_req);
            break;
        default:
        	LOG_ERROR("Recv error type");
            break;
        }
    }
    return;
}

void control_data_frame_handler(frame_req_t *frame_req)
{
    switch (frame_req->msg_type) {
    case CTRL_TIME_PKT:
    	LOG_INFO("Receive timestamp setting command");
        ctrl_timestamp_handler(frame_req);
        break;
    case CTRL_DEV_NET_INFO_PKT:
        LOG_INFO("Receive device net info setting command");
        ctrl_dev_net_handler(frame_req);
        break;
    case SUPERIOR_GET_DATA_PACK_TYPE:
        LOG_INFO("Receive superior get data command");
        superior_request_data_handler(frame_req);
        break;
    case CTRL_SAMPLE_INFO_PKT:
    	LOG_INFO("Receive ctrl sample info command");
        ctrl_sample_info_handler(frame_req);
        break;
    case CTRL_THRESHOLD_PKT:
        LOG_INFO("Receive device threshold setting command");
        ctrl_threshold_handler(frame_req);
        break;
    case CTRL_SERVER_NET_INFO_PKT:
        LOG_INFO("Receive remote server net info setting command");
        ctrl_server_net_info_hadler(frame_req);
        break;
    case CTRL_DEV_CFG_PKT:
        LOG_INFO("Receive device cfg command");
        ctrl_dev_cfg_handler(frame_req);
        break;
    case CTRL_DEV_UPGRADE_TRANSFER_PKT:
        LOG_INFO("Receive device upgrade file content");
        app_upgrade_handler(frame_req);
        break;
    case CTRL_DEV_UPGRADE_CNF_PKT:
        LOG_INFO("Receive device upgrade file transfer done confirm");
        app_upgrade_cnf_handler(frame_req);
        break;
    case COMPONENT_DEV_PACK_TYPE:
        LOG_INFO("Receive component dev ctrl command");
        component_dev_ctrl_handler(frame_req);
        break;
    case CTRL_DEV_RESET_PKT:
    	LOG_INFO("Receive dev reset command");
        ctrl_device_reset_handler(frame_req);
        break;
	case CTRL_DEV_IP_PACK_TYPE:
		LOG_INFO("Receive get dev ip addr command");
		ctrl_dev_ip(frame_req);
		break;
    default:
    	LOG_INFO("Get invalid message type: %02XH, please check again.", frame_req->msg_type);
        break;
    }
    return;
}

void frame_handler(frame_req_t *frame_req)
{
    switch (frame_req->frame_type) {
    case MONITOR_RSP_FRAME:
    	do_monitor_rsp_handler(frame_req);
        break;
    case CONTROL_DATA_FRAME:
        control_data_frame_handler(frame_req);
        break;
    case WORK_CONDITIN_REQ_FRAME:
        work_condition_frame_handler(frame_req);
        break;
    case 0x07:
    case 0x00:
    	break;
    default:
    	LOG_INFO("Get invalid frame type: %02XH, please check again.", frame_req->frame_type);
        break;
    }
    return;
}



