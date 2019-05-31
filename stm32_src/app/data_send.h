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

#define PERIOD_DATA_TYPE 0
#define PF_CURVE_TYPE 1
#define PF_PERIOD_DATA_TYPE 3
#define GENERAL_CALL_DATA_TYPE 4

#define MAX_FRAME_LEN 256
#define PACKET_DATA_LEN 200



#define CHANNEL_1 0
#define CHANNEL_2 1

#define SEND_PERIOD_TYPE 0
#define SEND_MUTATION 0
#define SEND_CALL 1

uint8_t get_send_type(void);

void set_server_call_flag(uint8_t flag);

kernel_pid_t data_send_serv_init(void);
void send_over_current_curve(over_current_data_t* over_current_data, uint8_t channel, uint8_t send_type);

#endif /* APP_DATA_SEND_H_ */
