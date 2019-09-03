/*
 * data_send.h
 *
 *  Created on: May 28, 2019
 *      Author: xuke
 */

#ifndef APP_DATA_SEND_H_
#define APP_DATA_SEND_H_

#include "kernel_types.h"
#include "over_current.h"

#define SEND_TASK_QUEUE_SIZE (16)

#define PERIOD_DATA_TYPE 0
#define CALL_DATA_TYPE 1
#define MUTATION_TYPE 2

#define MAX_FRAME_LEN 256
#define PACKET_DATA_LEN 200

#define CHANNEL_1 0
#define CHANNEL_2 1

#define SEND_PERIOD_TYPE 0
#define SEND_MUTATION 0
#define SEND_CALL 1

uint8_t get_send_type(void);

void set_server_call_flag(void);

void clear_server_call_flag(void);

uint8_t get_server_call_flag(void);

void send_dip_angle_data(void);

kernel_pid_t data_send_serv_init(void);
void send_over_current_curve(over_current_data_t* over_current_data, send_curve_info_t curve_info, uint8_t send_type);
#endif /* APP_DATA_SEND_H_ */
