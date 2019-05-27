#ifndef HEART_BEAT_H_
#define HEART_BEAT_H_
#include "thread.h"
#include "frame_common.h"
#include "periph/gpio.h"

#define STATUS_LED_PIN  GPIO_PIN(PORT_A,8)

typedef struct _register_info_t{
	uint8_t cmd_id[16];
	uint16_t error_count;
	uint8_t all_success_flag;
}register_info_t;

void comm_heart_beat_sender_hook(kernel_pid_t pid);

void send_heart_beat_thread_init(void);

void status_led_pin_init(void);

void send_register_msg(void);

void send_rtc_err(void);

void send_rtc_ok(void);

void init_register_info(void);

register_info_t * get_register_info(void);

void watch_dog_service_init(void);

#endif
