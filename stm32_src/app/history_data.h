/*
 * history_data.h
 *
 *  Created on: Mar 26, 2019
 *      Author: chenzy
 */

#ifndef SRC_HISTORY_DATA_H_
#define SRC_HISTORY_DATA_H_

#include "dev_cfg.h"


typedef struct _mx_history_data {
    float max;
    float min;
    float realtime;
    uint32_t timestamp;
} mx_history_data_t;

typedef struct _st_history_data {
    uint16_t defense;
    uint16_t status;
    uint32_t timestamp;
} st_history_data_t;

typedef struct _sc_history_data {
    uint16_t current;
    uint16_t alarm;
    uint32_t timestamp;
} sc_history_data_t;


history_count_t get_history_count_info(sensor_t type, phase_t phase);

void append_cur_history_data(mx_sensor_info_t mx, phase_t phase);
mx_history_data_t query_cur_history_data(phase_t phase, uint32_t idx);

void append_vol_history_data(mx_sensor_info_t mx, phase_t phase);
mx_history_data_t query_vol_history_data(phase_t phase, uint32_t idx);

void append_temp_history_data(mx_sensor_info_t mx, phase_t phase);
mx_history_data_t query_temp_history_data(phase_t phase, uint32_t idx);

void append_hfct_history_data(mx_sensor_info_t mx, phase_t phase);
mx_history_data_t query_hfct_history_data(phase_t phase, uint32_t idx);

void append_sc_alarm_history_data(sc_sensor_info_t sc, phase_t phase);
sc_history_data_t query_sc_alarm_history_data(phase_t phase, uint32_t index);

void append_alarm_history_data(st_sensor_info_t st, phase_t phase);
st_history_data_t query_alarm_history_data(uint32_t index);


#endif /* SRC_HISTORY_DATA_H_ */
