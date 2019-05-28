/*
 * period_data.h
 *
 *  Created on: May 28, 2019
 *      Author: xuke
 */

#ifndef APP_PERIOD_DATA_H_
#define APP_PERIOD_DATA_H_
#include "kernel_types.h"

kernel_pid_t period_data_serv_init(void);

void period_data_hook(kernel_pid_t pid);

#endif /* APP_PERIOD_DATA_H_ */
