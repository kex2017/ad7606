/*
 * sc_cfg.h
 *
 *  Created on: Mar 22, 2019
 *      Author: chenzy
 */

#ifndef SRC_SC_CFG_H_
#define SRC_SC_CFG_H_


#ifdef __cplusplus
extern "C" {
#endif

int cfg_command(int argc, char **argv);

void reset_cfg_command(void);

#ifdef __cplusplus
}
#endif


#endif /* SRC_SC_CFG_H_ */
