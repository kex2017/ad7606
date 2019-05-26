/*
 * acquisition_trigger.h
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_ACQUISITION_TRIGGER_H_
#define PARTS_CABLE_GROUND_NEW_ACQUISITION_TRIGGER_H_

#include <stdint.h>
#include "frame_common.h"

enum{
    FLAG_ON = 1,
    FLAG_OFF = -1
};

typedef enum {
    MSG_READ_COMMON_AND_PRPD,
    MSG_READ_COMMON,
    MSG_READ_PRPD,
    MSG_READ_PD,
    MSG_READ_OV,
} TASK_TYPE;



#define TRIGGER_REG (8)

typedef struct _msg_read_info {
    uint32_t channels;
    uint32_t timestamp;
} msg_read_info_t;


void init_sample_cfg(void);
void display_sample_info(void);
void set_channel_sample_cfg(channel_cfg_t cfg_info);
channel_cfg_t get_channel_sample_cfg(uint8_t channel);
uint8_t get_channel_gain_cfg(uint8_t channel);
void set_channel_gain_cfg(uint8_t channel, uint8_t gain);

uint32_t get_channel_sample_timestamp(uint8_t channel);
void set_channel_sample_timestamp(uint8_t channel, uint32_t timestamp);

void set_channel_data_sample_flag(uint8_t channel);
uint16_t get_channel_data_sample_flag(uint8_t channel);
void clear_channel_data_sample_flag(uint8_t channel);

void generate_pulse_signal(void);


void trigger_signal_by_hand(void);

void set_manual_sample_timer_trigger(uint32_t seconds, uint16_t data_type);


void pd_and_ov_event_irq_init(void);

int over_voltage_event_happen_notify(void);
void remove_pdirq_timer(void);
void remove_ovirq_timer(void);

#endif /* PARTS_CABLE_GROUND_NEW_ACQUISITION_TRIGGER_H_ */
