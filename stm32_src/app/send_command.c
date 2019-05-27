#include "send_command.h"
#include "frame_encode.h"
#include "periph/rtt.h"
#include "frame_handler.h"
#include "vc_temp_bat_vol_sample.h"
#include "data_transfer.h"
#include "heart_beat.h"

void do_send_fault_data(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    uint32_t time = rtt_get_counter();
    st_sensor_info_t* st = get_dev_alarm_info();

    length = device_fault_msg_encode(data, time, (uint8_t *)( "0101"), sizeof("0101"),st->cmd_id);

    msg_handler_packet_send(data, length);
}

void do_send_work_state(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    float voltage = (float)get_bat_vol();
    float current_time = get_current_runtime();
    float total_time = get_total_runtime();
    float temp = get_line_temp(0);
    int csq = get_ec20_csq_rssi();

	length = frame_work_condition_rsp_encode(data, get_dev_alarm_info()->cmd_id, voltage,
			temp, voltage / 12.6, 0x00, total_time, current_time, 0x00, csq);

	msg_handler_packet_send(data, length);
}

void do_send_dev_info(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

    product_info_t* dev_info =  get_dev_product_info();
	 st_sensor_info_t* dev_cfg = get_dev_alarm_info();

    length = frame_device_config_rsp_encode(data, dev_info, dev_cfg->cmd_id);

    msg_handler_packet_send(data, length);
}

void do_send_alarm_info(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    st_sensor_info_t* p = get_dev_alarm_info();

    length = frame_dev_alarm_encode(data, p);
    msg_handler_packet_send(data, length);
}

void do_send_hfct_info(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    mx_sensor_info_t* p = get_dev_hfct_info_by_phase(0);

    p->max = 111.1;
    p->min = 222.2;
    p->realtime = 333.3;
    p->timestamp = rtt_get_counter();
    length = frame_dev_hfct_encode(data, p);
    msg_handler_packet_send(data, length);
}

void do_send_vol_info(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    mx_sensor_info_t* p = get_dev_vol_info_by_phase(0);

    p->max = 444.4;
    p->min = 555.5;
    p->realtime = 666.6;
    p->timestamp = rtt_get_counter();
    length = frame_dev_vol_encode(data, p);
    msg_handler_packet_send(data, length);
}

void do_send_temp_info(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    mx_sensor_info_t* p = get_dev_temp_info_by_phase(0);

    p->max = 777.7;
    p->min = 888.8;
    p->realtime = 999.9;
    p->timestamp = rtt_get_counter();
    length = frame_temperature_encode(data, p);
    msg_handler_packet_send(data, length);
}

void do_send_cur_info(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    mx_sensor_info_t* p = get_dev_cur_info_by_phase(0);
    p->max = 999.9;
    p->min = 111.1;
    p->realtime = 333.3;
    p->timestamp = rtt_get_counter();
    length = frame_dev_cur_encode(data, p);
    msg_handler_packet_send(data, length);
}


int frame_command(int argc, char **argv)
{
	if (argc < 2) {
		return 1;
	}
	if (strncmp(argv[1], "fault", 5) == 0) {
		send_rtc_err();
	} else if (strncmp(argv[1], "work", 4) == 0) {
		do_send_work_state();
	} else if (strncmp(argv[1], "hfct", 4) == 0) {
		do_send_hfct_info();
	} else if (strncmp(argv[1], "alarm", 5) == 0) {
		do_send_alarm_info();
	} else if (strncmp(argv[1], "vol", 3) == 0) {
		do_send_vol_info();
	} else if (strncmp(argv[1], "temp", 4) == 0) {
		do_send_temp_info();
	} else if (strncmp(argv[1], "cur", 3) == 0) {
		do_send_cur_info();
	} else if (strncmp(argv[1], "dev", 3) == 0) {
		do_send_dev_info();
	} else if (strncmp(argv[1], "recover", 7) == 0) {
		send_rtc_ok();
	}
	return 0;
}
