/*
 * curve_harmonics.h
 *
 *  Created on: May 16, 2018
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_CURVE_HARMONICS_H_
#define PARTS_CABLE_GROUND_CURVE_HARMONICS_H_

#include <stdint.h>
#include "arm_math.h"

#define MAX_HARMONICS    (13)

typedef struct _channel_harmonics_info {
   float32_t base_current;  //unused
   float32_t base_phase;
   float32_t harmonics[MAX_HARMONICS];
} channel_harmonics_info_t;


channel_harmonics_info_t* get_harmonics_info_by_channel(uint8_t channel);
void calc_dieloss_harmonics(int16_t *data, uint8_t channel);
void show_last_sampled_harmonics_info(uint8_t channel);

#endif /* PARTS_CABLE_GROUND_CURVE_HARMONICS_H_ */
