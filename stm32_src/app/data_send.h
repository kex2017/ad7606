/*
 * data_send.h
 *
 *  Created on: May 28, 2019
 *      Author: xuke
 */

#ifndef APP_DATA_SEND_H_
#define APP_DATA_SEND_H_

#include "kernel_types.h"

#define PERIOD_DATA_TYPE 0
#define PF_CURVE_TYPE 1

#define MAX_FRAME_LEN 256
#define PACKET_DATA_LEN 200

#define CHANNEL_1 0x01
#define CHANNEL_2 0x02


kernel_pid_t data_send_serv_init(void);
void send_over_current_curve(over_current_data_t* over_current_data, uint8_t channel);

#endif /* APP_DATA_SEND_H_ */
