#include "history_data_send.h"
#include "thread.h"
#include "msg.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include "frame_handler.h"
#include "frame_common.h"
#include "frame_encode.h"
#include "history_data.h"
#include "data_transfer.h"
#include "periph/rtt.h"
#include "heart_beat.h"
#include "dev_cfg.h"
#include "ec20_at.h"
#include "type_alias.h"
#include "xtimer.h"
#include "timex.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static void delay_s(int s)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
}

//static void delay_ms(int ms)
//{
//	xtimer_ticks32_t last_wakeup = xtimer_now();
//	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
//}

kernel_pid_t comm_packet_receiver_pid = KERNEL_PID_UNDEF;

#define COMM_PACKET_RECEIVERSTACKSIZE          (THREAD_STACKSIZE_MAIN)
static uint8_t data_type;
static uint32_t history_start_time = 0;
static uint32_t history_end_time = 0;

static uint32_t time ;

void set_history_time_interval(uint32_t start_time , uint32_t end_time, uint8_t type)
{
	history_start_time = start_time;
	history_end_time = end_time;
	data_type = type;
}

void clear_history_time_interval(void)
{
	history_start_time = 0;
	history_end_time = 0;
	data_type = 0;
}

static bl_time_t bl_time;

bl_time_t *get_bl_time(void)
{
	return &bl_time;
}

void bl_time_init(void)
{
	bl_time.break_time = 0;
	bl_time.linked_time = 0;
	bl_time.dev_link_breaked_flag = 0;
}

//*************************HISTORY DATA START******************************
#define HISTORY_DATA_SEND_PRIO               (THREAD_PRIORITY_MAIN - 2)
kernel_pid_t history_data_send_pid = KERNEL_PID_UNDEF;

#define HISTORY_DATA_SEND_STACKSIZE          (THREAD_STACKSIZE_MAIN)
static char history_data_send_stack[HISTORY_DATA_SEND_STACKSIZE*2];

void send_current_history_data(uint32_t start_time , uint32_t end_time, int phase)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	mx_sensor_info_t* current = get_dev_cur_info_by_phase(phase);
	history_count_t history;
	mx_history_data_t history_data;

	if (current->exist_flag) {
		history = get_history_count_info(CUR, phase);
		for (uint32_t index = 0; index < history.total_num; index++) {
			history_data = query_cur_history_data(phase,
					index);
			if ((history_data.timestamp >= start_time - time)
					&& (history_data.timestamp <= end_time)) {
				current->max = history_data.max;
				current->min = history_data.min;
				current->realtime = history_data.realtime;
				current->timestamp = history_data.timestamp;
				length = frame_dev_cur_encode(data, current);
				msg_handler_packet_send(data, length);
                delay_s(2);
			}
		}
	}
}

void send_voltage_history_data(uint32_t start_time , uint32_t end_time, int phase)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	mx_sensor_info_t* voltage = get_dev_vol_info_by_phase(phase);
	history_count_t history;
	mx_history_data_t history_data;
	if (voltage->exist_flag) {
		history = get_history_count_info(VOL, phase);
		for (uint32_t index = 0; index < history.total_num; index++) {
			history_data = query_vol_history_data(phase,
					index);
			if ((history_data.timestamp >= start_time - time)
					&& (history_data.timestamp <= end_time)) {
				voltage->max = history_data.max;
				voltage->min = history_data.min;
				voltage->realtime = history_data.realtime;
				voltage->timestamp = history_data.timestamp;
				length = frame_dev_vol_encode(data, voltage);
				msg_handler_packet_send(data, length);
                delay_s(2);
			}
		}
	}
}

void send_temp_history_data(uint32_t start_time , uint32_t end_time, int phase)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	mx_sensor_info_t* temp = get_dev_temp_info_by_phase(phase);
	history_count_t history;
	mx_history_data_t history_data;
	if (temp->exist_flag) {
		history = get_history_count_info(TEMP, phase);
		for (uint32_t index = 0; index < history.total_num; index++) {
			 history_data = query_temp_history_data(phase,
					index);
			if ((history_data.timestamp >= start_time - time)
					&& (history_data.timestamp <= end_time)) {
				temp->max = history_data.max;
				temp->min = history_data.min;
				temp->realtime = history_data.realtime;
				temp->timestamp = history_data.timestamp;
				length = frame_temperature_encode(data, temp);
				msg_handler_packet_send(data, length);
                delay_s(2);
			}
		}
	}
}

void send_hfct_history_data(uint32_t start_time , uint32_t end_time, int phase)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	mx_sensor_info_t* hfct = get_dev_hfct_info_by_phase(phase);
	history_count_t history;
	mx_history_data_t history_data;
	if(hfct->exist_flag)
	{
		history = get_history_count_info(HFCT, phase);
		for(uint32_t index = 0; index < history.total_num; index ++)
		{
			history_data = query_hfct_history_data(phase,
					index);
			if ((history_data.timestamp >= start_time - time)
					&& (history_data.timestamp <= end_time)) {
				hfct->max = history_data.max;
				hfct->min = history_data.min;
				hfct->realtime = history_data.realtime;
				hfct->timestamp = history_data.timestamp;
				length = frame_dev_hfct_encode(data, hfct);
				msg_handler_packet_send(data, length);
                delay_s(2);
			}
		}
	}
}

void send_sc_alarm_history_data(uint32_t start_time , uint32_t end_time, int phase)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    sc_sensor_info_t* sc_alarm = get_dev_sc_alarm_info_by_phase(phase);
    history_count_t history;
    sc_history_data_t history_data;

    if (sc_alarm->exist_flag) {
        history = get_history_count_info(SC_ALARM, phase);
        for (uint32_t index = 0; index < history.total_num; index++) {
        	history_data   = query_sc_alarm_history_data(phase, index);
            if ((history_data.timestamp >= start_time - time) && (history_data.timestamp <= end_time)) {
                sc_alarm->current = history_data.current;
                sc_alarm->alarm = !!history_data.alarm;
                sc_alarm->timestamp = history_data.timestamp;
                length = frame_dev_sc_alarm_encode(data, sc_alarm);
                msg_handler_packet_send(data, length);
                delay_s(2);
            }
        }
    }
}

void send_alarm_history_data(uint32_t start_time , uint32_t end_time, int phase)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	history_count_t history;
	st_sensor_info_t *sensor_data = get_dev_alarm_info();
	st_history_data_t history_data;

	if(phase == 0){
		history = get_history_count_info(ALARM, phase);
		for(uint32_t index = 0; index < history.total_num; index ++)
		{
			history_data  = query_alarm_history_data(index);
			if ((history_data.timestamp >= start_time - time)
					&& (history_data.timestamp <= end_time)) {
			    sensor_data->defense = history_data.defense;
			    sensor_data->status = history_data.status;
			    sensor_data->timestamp = history_data.timestamp;
			    length = frame_dev_alarm_encode(data, sensor_data);
				msg_handler_packet_send(data, length);
                delay_s(2);
			}
		}
	}
}

void send_history_data(uint32_t start_time, uint32_t end_time,sensor_t type)
{
	int phase;

	for (phase = 0; phase < 3; phase++) {
		switch(type){
		case CUR:
			send_current_history_data(start_time, end_time, phase);
			break;
		case VOL:
			send_voltage_history_data(start_time, end_time, phase);
			break;
		case TEMP:
			send_temp_history_data(start_time, end_time, phase);
			break;
		case HFCT:
			send_hfct_history_data(start_time, end_time, phase);
			break;
		case ALARM:
			send_alarm_history_data(start_time, end_time, phase);
			break;
		case SC_ALARM:
		    send_sc_alarm_history_data(start_time, end_time, phase);
		    break;
		}
	}
	clear_history_time_interval();
	LOG_INFO("Send history data over");
}

void do_send_history_data(uint32_t start_time , uint32_t end_time, uint8_t data_type)
{
	switch (data_type) {
	case TEMP_DATA_UPLOAD_PKT:
		LOG_INFO("Start send temp history data\r\n");
		send_history_data(start_time, end_time, 2);
		break;
	case CUR_DATA_UPLOAD_PKT:
		LOG_INFO("Start send current history data\r\n");
		send_history_data(start_time, end_time, 0);
		break;
	case HFCT_DATA_UPLOAD_PKT:
		LOG_INFO("Start send htfc history data\r\n");
		send_history_data(start_time, end_time, 3);
		break;
	case VOL_DATA_UPLOAD_PKT:
		LOG_INFO("Start send voltage history data\r\n");
		send_history_data(start_time, end_time, 1);
		break;
	case ALARM_STATUS_UPLOAD_PKT:
		LOG_INFO("Start send alarm history data\r\n");
		send_history_data(start_time, end_time, 5);
		break;
	case SHORT_CIRCUIT_ALARM_UPLOAD_PKT:
		LOG_INFO("Start send sc alarm history data\r\n");
		send_history_data(start_time, end_time, 4);
		break;
	default:
		LOG_INFO("Error history data type\r\n");
		break;
	}
}

void send_disconnect_miss_data(uint32_t start_time , uint32_t end_time)
{
	mx_sensor_info_t* cur_info = get_dev_cur_info_by_phase(0);
	time  =  cur_info->interval * 60*2;
	for(int phase = 0; phase<3; phase++)
	{
		send_current_history_data(start_time, end_time, phase);
		send_voltage_history_data(start_time, end_time, phase);
//		send_temp_history_data(start_time, end_time, phase);
		send_hfct_history_data(start_time, end_time, phase);
	    send_sc_alarm_history_data(start_time, end_time, phase);
	}
	send_alarm_history_data(start_time, end_time, 0);
	bl_time_init();
	LOG_INFO("Send disconnect data over");
}

void* history_transfer(void *arg)
{
	(void) arg;
	msg_t msg;
	bl_time_init();
	init_register_info();
	while (1) {
		msg_receive(&msg);
		if (msg.type == DSC_DATA) {
			LOG_INFO("Start send disconnect miss data");
			send_disconnect_miss_data(bl_time.break_time, bl_time.linked_time);
		} else if (msg.type == HIS_DATA) {
			do_send_history_data(history_start_time, history_end_time,
					data_type);
		} else if (msg.type == LINK) {
			LOG_INFO("Start send register");
			send_register_msg();
		}else if (msg.type == BREAK) {
				if (bl_time.dev_link_breaked_flag == 0 && get_register_info()->all_success_flag == 1) {
					bl_time.dev_link_breaked_flag = 1;
					bl_time.break_time = rtt_get_counter();
				}
			}
			delay_s(2);
		}
		return NULL;
}


#define HISTORY_DATA_SEND_PRIORITY (9)
kernel_pid_t history_data_send_init(void)
{
	if (history_data_send_pid == KERNEL_PID_UNDEF) {
		history_data_send_pid = thread_create(history_data_send_stack,
				sizeof(history_data_send_stack),
				HISTORY_DATA_SEND_PRIORITY, THREAD_CREATE_STACKTEST,
				history_transfer, NULL, "history data send");
	}
	history_data_send_pid_hook(history_data_send_pid);
	history_hook(history_data_send_pid);
	return history_data_send_pid;
}

//*************************HISTORY DATA END******************************
