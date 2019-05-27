/*
 * data_processor.h
 *
 *  Created on: Apr 16, 2019
 *      Author: xuke
 */

#ifndef VC_TEMP_BAT_SAMPLE_H_
#define VC_TEMP_BAT_SAMPLE_H_
#include "thread.h"

typedef enum{
    LINEA = 0,
    LINEB = 1,
    LINEC = 2
}line_temperature_t;

#define SAMPLE_COUNT 64
#define CHANNEL_COUNT 9
#define SAMPLE_DURATION_MS 20

#define TEMP_ADC2_CHAN_CNT 2
#define TEMP_BAT_ADC3_CHAN_CNT 2

kernel_pid_t vc_temp_bat_sample_serv_init(void);

uint16_t get_line_temp(line_temperature_t line);
void  do_receive_pid_hook(kernel_pid_t pid);
uint16_t get_bat_vol(void);



#endif /* SRC_VC_TEMP_BAT_VOL_SAMPLE_H_ */
