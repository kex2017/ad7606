#include "heart_beat.h"
#include "frame_encode.h"
#include "thread.h"
#include "periph/rtt.h"
#include "frame_common.h"
#include "dev_cfg.h"
#include "board.h"
#include "data_transfer.h"
#include "app_upgrade.h"
#include "timex.h"
#include "log.h"
#include "xtimer.h"
#include "periph/stm32f10x_std_periph.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

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
//static PACKET send_heart_beat_packet = { 0 };
//static packet_t send_heart_beat_packet;
//static msg_t heart_beat_msg;
static kernel_pid_t heart_beat_sender_pid;
static register_info_t register_info;
void comm_heart_beat_sender_hook(kernel_pid_t pid)
{
   heart_beat_sender_pid = pid;
}

void status_led_pin_init(void)
{
    gpio_init(STATUS_LED_PIN, GPIO_OUT);
}

void status_led_on(void)
{
    gpio_set(STATUS_LED_PIN);
}

void status_led_off(void)
{
    gpio_clear(STATUS_LED_PIN);
}

void do_send_heart_beat_packet(uint8_t *data, uint32_t data_len)
{
	if (get_ec20_link_flag() == LINK_UP) {
		while (set_data_to_cache(data, data_len) == 0) {
			delay_ms(500);
		}
	}
}

void init_register_info(void)
{
	register_info.error_count = 13;
	register_info.all_success_flag = 0;
	for(uint8_t i = 0; i< 13; i++)
	{
		register_info.cmd_id[i] = i;
	}
}

void resend_register_msg_by_cmd_id(uint8_t index, uint8_t type)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	product_info_t* dev_info = get_dev_product_info();
	mx_sensor_info_t* mx_register_info;
	st_sensor_info_t* st_register_info;
	sc_sensor_info_t* sc_register_info;

	switch (type) {
	case 'C':
		mx_register_info = get_dev_cur_info_by_phase(index);
		length = frame_device_config_rsp_encode(data, dev_info, mx_register_info->cmd_id);
		do_send_heart_beat_packet(data, length);
		break;
	case 'V':
		mx_register_info = get_dev_vol_info_by_phase(index - 3);
		length = frame_device_config_rsp_encode(data, dev_info, mx_register_info->cmd_id);
		do_send_heart_beat_packet(data, length);
		break;
//	case 'T':
//		mx_register_info = get_dev_temp_info_by_phase(index - 6);
//		length = frame_device_config_rsp_encode(data, dev_info, mx_register_info->cmd_id);
//		do_send_heart_beat_packet(data, length);
//		break;
	case 'H':
		mx_register_info = get_dev_hfct_info_by_phase(index - 9);
		length = frame_device_config_rsp_encode(data, dev_info, mx_register_info->cmd_id);
		do_send_heart_beat_packet(data, length);
		break;
	case 'S':
		sc_register_info = get_dev_sc_alarm_info_by_phase(index - 12);
		length = frame_device_config_rsp_encode(data, dev_info, sc_register_info->cmd_id);
		do_send_heart_beat_packet(data, length);
		break;
	case 'A':
		st_register_info = get_dev_alarm_info();
		length = frame_device_config_rsp_encode(data, dev_info, st_register_info->cmd_id);
		do_send_heart_beat_packet(data, length);
		break;
	}
}

void resend_register_info(void)
{
	for (int i = 0; i < 16; i++) {
		if (register_info.cmd_id[i] != 0) {
			if (i < 3) {
				resend_register_msg_by_cmd_id(i, 'C');
			}
			if (i < 6 && i >=3) {
				resend_register_msg_by_cmd_id(i, 'V');
			}
//			if (i < 9 && i >= 6) {
//				resend_register_msg_by_cmd_id(i, 'T');
//			}
			if (i < 12 && i >= 9) {
				resend_register_msg_by_cmd_id(i, 'H');
			}
			if (i < 15 && i >= 12) {
				resend_register_msg_by_cmd_id(i, 'S');
			}
			if(i == 15) {
				resend_register_msg_by_cmd_id(i, 'A');
			}
		}
	}
}

register_info_t * get_register_info(void)
{
	return &register_info;
}

void send_register_msg(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	product_info_t* dev_info = get_dev_product_info();
	init_register_info();

	for (int i = 0; i < 3; i++) {
		mx_sensor_info_t* current = get_dev_cur_info_by_phase(i);
		length = frame_device_config_rsp_encode(data, dev_info, current->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
	memset(data, 0, sizeof(data));
	for (int i = 0; i < 3; i++) {
		mx_sensor_info_t* vol = get_dev_vol_info_by_phase(i);
		length = frame_device_config_rsp_encode(data, dev_info, vol->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
//	memset(data, 0, sizeof(data));
//	for (int i = 0; i < 3; i++) {
//		mx_sensor_info_t* temp = get_dev_temp_info_by_phase(i);
//		length = frame_device_config_rsp_encode(data, dev_info, temp->cmd_id);
//		do_send_heart_beat_packet(data, length);
//		delay_s(5);
//	}
	memset(data, 0, sizeof(data));
	for (int i = 0; i < 3; i++) {
		mx_sensor_info_t* htfc = get_dev_hfct_info_by_phase(i);
		length = frame_device_config_rsp_encode(data, dev_info, htfc->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
	memset(data, 0, sizeof(data));
	for (int i = 0; i < 3; i++) {
		sc_sensor_info_t *sc = get_dev_sc_alarm_info_by_phase(i);
		length = frame_device_config_rsp_encode(data, dev_info, sc->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
	memset(data, 0, sizeof(data));
	st_sensor_info_t* st = get_dev_alarm_info();

	length = frame_device_config_rsp_encode(data, dev_info, st->cmd_id);
	do_send_heart_beat_packet(data, length);
	LOG_INFO("Send device register over");
}

void send_sensor_heart_beat(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

	for (int i = 0; i < 3; i++) {
		mx_sensor_info_t* current = get_dev_cur_info_by_phase(i);
		length = frame_heart_beat_rsp_encode(data, rtt_get_counter(), current->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
	memset(data, 0, sizeof(data));
	for (int i = 0; i < 3; i++) {
		mx_sensor_info_t* vol = get_dev_vol_info_by_phase(i);
		length = frame_heart_beat_rsp_encode(data, rtt_get_counter(), vol->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
//	memset(data, 0, sizeof(data));
//	for (int i = 0; i < 3; i++) {
//		mx_sensor_info_t *temp = get_dev_temp_info_by_phase(i);
//		length = frame_heart_beat_rsp_encode(data, rtt_get_counter(), temp->cmd_id);
//		do_send_heart_beat_packet(data, length)
//		delay_s(5);
//	}
	memset(data, 0, sizeof(data));
	for (int i = 0; i < 3; i++) {
		mx_sensor_info_t* htfc = get_dev_hfct_info_by_phase(i);
		length = frame_heart_beat_rsp_encode(data, rtt_get_counter(), htfc->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
	memset(data, 0, sizeof(data));
	for (int i = 0; i < 3; i++) {
		sc_sensor_info_t* sc = get_dev_sc_alarm_info_by_phase(i);
		length = frame_heart_beat_rsp_encode(data, rtt_get_counter(), sc->cmd_id);
		do_send_heart_beat_packet(data, length);
		delay_s(5);
	}
	memset(data, 0, sizeof(data));
	st_sensor_info_t* st = get_dev_alarm_info();

	length = frame_heart_beat_rsp_encode(data, rtt_get_counter(), st->cmd_id);
	do_send_heart_beat_packet(data, length);

//	DEBUG("keep alive link break num is %d\r\n", break_times);
}

void send_rtc_err(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };

	st_sensor_info_t* st = get_dev_alarm_info();

	length = device_fault_msg_encode(data, rtt_get_counter(),(uint8_t *)("0101timefault"), sizeof("0101timefault"),st->cmd_id);
	do_send_heart_beat_packet(data, length);
}

void send_rtc_ok(void)
{
	uint16_t length = 0;
	uint8_t data[MAX_REQ_FRAME_LEN] = { 0 };
	st_sensor_info_t* st = get_dev_alarm_info();

	length = device_fault_msg_encode(data, rtt_get_counter(),(uint8_t *) ( "0181recover"), sizeof("0181recover"),st->cmd_id);
	do_send_heart_beat_packet(data, length);

}

void *send_heart_beat_handler(void* arg)
{
	(void) arg;
	static uint32_t minute = 0;
	static int j = 0;
	status_led_pin_init();
	while (1) {
        if (get_ec20_link_flag() == LINK_BREAK) {
            delay_ms(500);
            continue;
        }
		status_led_on();
		delay_s(1);

		status_led_off();
		delay_s(59);
		minute++;
		if (minute % 60 == 0) {
			update_runtime_info(1);
		}

		if (register_info.all_success_flag != 1) {
			resend_register_info();
		}else
		{
			LOG_INFO("Keep a live");
			send_sensor_heart_beat();
		}

		if (rtt_get_counter() < 100000) {
			send_rtc_err();
			j = 1;
		} else {
			if (j == 1) {
				send_rtc_ok();
				j++;
			}
		}
	}
}

#define SEND_HERRT_BEAT_PRIORITY (8)
static char send_heart_beat_thread_stack[THREAD_STACKSIZE_MAIN];
void send_heart_beat_thread_init(void) {
	DEBUG("Send heart beat thread start....\r\n");
	kernel_pid_t _pid;
	_pid = thread_create(send_heart_beat_thread_stack,
			sizeof(send_heart_beat_thread_stack), SEND_HERRT_BEAT_PRIORITY,
			THREAD_CREATE_STACKTEST, send_heart_beat_handler, NULL,
			"send_heart_beat");
	(void) _pid;
}

/*--------watch_dog_service---------*/
void* watch_dog_service(void* argv)
{
    (void)argv;
    iwdg_init();
    while (1) {
        delay_s(10);
        iwdg_feed();
        LOG_DEBUG("feed watch dog ...");
    }
}
#define WATCH_DOG_PRIORITY (11)
static char watch_dog_thread_stack[THREAD_STACKSIZE_DEFAULT - 1024];
void watch_dog_service_init(void)
{
    thread_create(watch_dog_thread_stack, sizeof(watch_dog_thread_stack),
                  WATCH_DOG_PRIORITY,
                  THREAD_CREATE_STACKTEST, watch_dog_service, NULL, "watch_dog_service");
}


