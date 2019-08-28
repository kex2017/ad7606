/*
 * sc_device_cfg_test.h
 *
 *  Created on: Mar 19, 2018
 *      Author: chenzy
 */

#ifndef PARTS_FAULT_PARTITION_SC_DEVICE_CFG_TEST_H_
#define PARTS_FAULT_PARTITION_SC_DEVICE_CFG_TEST_H_

#include "env_cfg.h"

int set_device_cfg(int argc, char **argv);
int printenv_command(int argc, char **argv);
int send_command(int argc, char **argv);

#endif /* PARTS_FAULT_PARTITION_SC_DEVICE_CFG_TEST_H_ */
