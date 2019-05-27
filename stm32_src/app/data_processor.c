#include "data_processor.h"
#include "thread.h"
#include "data_transfer.h"
#include "vc_temp_bat_vol_sample.h"
#include "msg.h"
#include "timex.h"
#include "xtimer.h"
#include "frame_common.h"
#include "dev_cfg.h"
#include "periph/rtt.h"
#include "daq.h"
#include "frame_encode.h"
#include "history_data.h"
#include "frame_handler.h"
#include "switch_value.h"

static void delay_ms(int ms)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}
uint8_t data_send_flag[13];

static void delay_s(int s)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC * s);
}

void  clear_data_send_flag(void)
{
	for(int i = 0; i< 13; i++)
	{
		data_send_flag[i]  = 0;
	}
}

uint8_t *get_data_send_flag(void)
{
	return data_send_flag;
}

void set_alarm_dev_info(uint32_t timestamp, int phase)
{
	uint8_t index;
	(void) phase;
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	st_sensor_info_t* alarm_info = get_dev_alarm_info();
	alarm_info->status  = 0;
	for(index = 1; index<6; index ++){
		  if(!read_sw(index)){
			  switch(index)  {
			  case 1:
				  alarm_info->status  += 0x01;
				  break;
			  case 2:
				  alarm_info->status += 0x02;
				  break;
			  case 3:
				  alarm_info->status += 0x04;
				  break;
			  case 4:
				  alarm_info->status += 0x08;
				  break;
			  case 5:
				  alarm_info->status += 0x10;
				  break;
			  }
		  }
	}
	alarm_info->timestamp = timestamp;
	append_alarm_history_data(*alarm_info, phase);
	length = frame_dev_alarm_encode(data, alarm_info);
    msg_handler_packet_send(data, length);
	delay_s(3);
}

int check_send_all_data_success(float * rs_data, uint32_t timestamp )
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
    mx_sensor_info_t* voltage = NULL;
    mx_sensor_info_t* current = NULL;
    mx_sensor_info_t* htfc = NULL;
	sc_sensor_info_t* temp = NULL;
	for(int i =0; i< 13; i++)
	{
		if (data_send_flag[i] == 0) {
			switch (i) {
			case 0:
				set_alarm_dev_info(timestamp, 0);
				break;
			case 1:
				voltage = get_dev_vol_info_by_phase(0);
				voltage->realtime = (float) rs_data[0];
				voltage->timestamp = timestamp;
				if ((float) rs_data[0] > voltage->max) {
					voltage->max = (float) rs_data[0];
				} else if ((float) rs_data[0] < voltage->min) {
					voltage->min = (float) rs_data[0];
				}
				length = frame_dev_vol_encode(data, voltage);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 2:
				voltage = get_dev_vol_info_by_phase(1);
				voltage->realtime = (float) rs_data[1];
				voltage->timestamp = timestamp;
				if ((float) rs_data[1] > voltage->max) {
					voltage->max = (float) rs_data[1];
				} else if ((float) rs_data[1] < voltage->min) {
					voltage->min = (float) rs_data[1];
				}
				length = frame_dev_vol_encode(data, voltage);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 3:
				voltage = get_dev_vol_info_by_phase(2);
				voltage->realtime = (float) rs_data[2];
				voltage->timestamp = timestamp;
				if ((float) rs_data[2] > voltage->max) {
					voltage->max = (float) rs_data[2];
				} else if ((float) rs_data[2] < voltage->min) {
					voltage->min = (float) rs_data[2];
				}
				length = frame_dev_vol_encode(data, voltage);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 4:
				temp = get_dev_sc_alarm_info_by_phase(0);

				temp->current = (uint16_t) rs_data[3];
				temp->timestamp = timestamp;
				if (rs_data[5] > 500) {
					temp->alarm = 1;
				} else {
					temp->alarm = 0;
				}
				length = frame_dev_sc_alarm_encode(data, temp);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 5:
				temp = get_dev_sc_alarm_info_by_phase(1);

				temp->current = (uint16_t) rs_data[4];
				temp->timestamp = timestamp;
				if (rs_data[5] > 500) {
					temp->alarm = 1;
				} else {
					temp->alarm = 0;
				}
				length = frame_dev_sc_alarm_encode(data, temp);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 6:
				temp = get_dev_sc_alarm_info_by_phase(2);

				temp->current = (uint16_t) rs_data[5];
				temp->timestamp = timestamp;
				if (rs_data[5] > 500) {
					temp->alarm = 1;
				} else {
					temp->alarm = 0;
				}
				length = frame_dev_sc_alarm_encode(data, temp);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 7:
				current = get_dev_cur_info_by_phase(0);
				current->realtime = (float) rs_data[6];
				current->timestamp = timestamp;
				if ((float) rs_data[6] > current->max) {
					current->max = (float) rs_data[6];
				} else if ((float) rs_data[6] < current->min) {
					current->min = (float) rs_data[6];
				}
				length = frame_dev_cur_encode(data, current);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 8:
				current = get_dev_cur_info_by_phase(1);
				current->realtime = (float) rs_data[7];
				current->timestamp = timestamp;
				if ((float) rs_data[7] > current->max) {
					current->max = (float) rs_data[7];
				} else if ((float) rs_data[7] < current->min) {
					current->min = (float) rs_data[7];
				}
				length = frame_dev_cur_encode(data, current);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 9:
				current = get_dev_cur_info_by_phase(2);
				current->realtime = (float) rs_data[8];
				current->timestamp = timestamp;
				if ((float) rs_data[8] > current->max) {
					current->max = (float) rs_data[8];
				} else if ((float) rs_data[8] < current->min) {
					current->min = (float) rs_data[8];
				}
				length = frame_dev_cur_encode(data, current);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 10:
				htfc = get_dev_hfct_info_by_phase(0);
				htfc->freq = daq_spi_get_discharge_num(0);
				htfc->timestamp = timestamp;
				htfc->max = (float) daq_spi_get_dat_max(0);
				length = frame_dev_hfct_encode(data, htfc);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 11:
				htfc = get_dev_hfct_info_by_phase(1);
				htfc->freq = daq_spi_get_discharge_num(1);
				htfc->timestamp = timestamp;
				htfc->max = (float) daq_spi_get_dat_max(1);
				length = frame_dev_hfct_encode(data, htfc);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			case 12:
				htfc = get_dev_hfct_info_by_phase(2);
				htfc->freq = daq_spi_get_discharge_num(2);
				htfc->timestamp = timestamp;
				htfc->max = (float) daq_spi_get_dat_max(2);
				length = frame_dev_hfct_encode(data, htfc);
				msg_handler_packet_send(data, length);
				delay_s(1);
				break;
			}
			return 0;
		}
	}
	return 1;
}

void set_current_dev_info(int phase,float rms_data,uint32_t timestamp)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

	mx_sensor_info_t* current = get_dev_cur_info_by_phase(phase);
	current->realtime = (float) rms_data;
	current->timestamp = timestamp;
	if ((float) rms_data > current->max) {
		current->max = (float) rms_data;
	} else if ((float) rms_data < current->min) {
		current->min = (float) rms_data;
	}
	append_cur_history_data(*current, phase);
    length = frame_dev_cur_encode(data, current);
    msg_handler_packet_send(data, length);
    delay_s(3);
}

void set_voltage_dev_info(int phase,float rms_data,uint32_t timestamp)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	mx_sensor_info_t* voltage = get_dev_vol_info_by_phase(phase);
	voltage->realtime = (float) rms_data;
	voltage->timestamp = timestamp;
	if ((float) rms_data > voltage->max) {
		voltage->max = (float) rms_data;
	} else if ((float) rms_data < voltage->min) {
		voltage->min = (float) rms_data;
	}
	append_vol_history_data(*voltage, phase);
    length = frame_dev_vol_encode(data, voltage);
    msg_handler_packet_send(data, length);
    delay_s(3);
}

void set_htfc_dev_info(int phase,uint32_t rms_data,uint32_t timestamp)
{
	(void) rms_data;
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

	mx_sensor_info_t* htfc = get_dev_hfct_info_by_phase(phase);

	htfc->freq =  daq_spi_get_discharge_num(phase);
	htfc->timestamp = timestamp;
	htfc->max =(float)daq_spi_get_dat_max(phase);

	append_hfct_history_data(*htfc, phase);
    length = frame_dev_hfct_encode(data, htfc);
    msg_handler_packet_send(data, length);
    delay_s(3);
}

void set_temp_dev_info(int phase,uint32_t timestamp)
{
    uint16_t length = 0;
    uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	mx_sensor_info_t* temp = get_dev_temp_info_by_phase(phase);

	float real_time_temp = (float) get_line_temp(phase);
	temp->realtime = real_time_temp;
	temp->timestamp = timestamp;
	if (real_time_temp > temp->max) {
		temp->max = real_time_temp;
	} else if (real_time_temp < temp->min) {
		temp->min = real_time_temp;
	}
	append_temp_history_data(*temp, phase);
    length = frame_temperature_encode(data, temp);
    msg_handler_packet_send(data, length);
	delay_s(3);
}

void set_dev_sc_alarm_info(int phase, float  sc_data, uint32_t timestamp)
{
	uint32_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

	sc_sensor_info_t* temp = get_dev_sc_alarm_info_by_phase(phase);

	temp->current = (uint16_t )sc_data;
	temp->timestamp = timestamp;
	if (sc_data > 500) {
		temp->alarm = 1;
	} else {
		temp->alarm = 0;
	}
	append_sc_alarm_history_data(*temp,phase);
	length = frame_dev_sc_alarm_encode(data, temp);
	msg_handler_packet_send(data, length);
	delay_s(3);
}

void upgrade_all_dev_data_info(float* rms_data)
{
    int phase;
    uint32_t timestamp = rtt_get_counter();
    clear_data_send_flag();
    set_alarm_dev_info(timestamp, 0);
    for (phase = 0; phase < 3; phase++) {
        set_voltage_dev_info(phase, rms_data[phase], timestamp);
        set_dev_sc_alarm_info(phase, rms_data[phase+3], timestamp);
        set_current_dev_info(phase, rms_data[phase + 6], timestamp);
        set_htfc_dev_info(phase, rms_data[phase + 6], timestamp);
//        set_temp_dev_info(phase, timestamp);
    }
    delay_s(5);
    for(int i = 0; i<10; i++)
    {
    	if(check_send_all_data_success(rms_data, timestamp))
    	{
    		LOG_INFO("Send all data success");
    		break;
    	}
    	delay_s(10);
    }
}

void save_rms_data(msg_t * msg,  float* rms)
{
	memcpy(rms, msg->content.ptr, sizeof(float)*9);
}

void *data_processor(void *arg)
{
    (void)arg;
    msg_t recv_msg;
    float rms_data[9];
    while (1) {
        msg_receive(&recv_msg);
        save_rms_data(&recv_msg,rms_data);
        upgrade_all_dev_data_info(rms_data);
        LOG_INFO("Save data success");
        delay_ms(500);
    }
}

#define DATA_PROCESSOR_PRIORITY (6)
static char data_processor_thread_stack[THREAD_STACKSIZE_MAIN*2];
void data_processor_thread_init(void)
{
    LOG_INFO("Data processor thread start....\r\n");
    kernel_pid_t _pid = thread_create(data_processor_thread_stack, sizeof(data_processor_thread_stack),
                                      DATA_PROCESSOR_PRIORITY,
                                      THREAD_CREATE_STACKTEST, data_processor, NULL, "data_process");
    								do_receive_pid_hook(_pid);
   (void)_pid;
}
