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
#define HF_CURVE_TYPE 2

#define CHANNEL_1 0x01
#define CHANNEL_2 0x02


kernel_pid_t data_send_serv_init(void);

#endif /* APP_DATA_SEND_H_ */
