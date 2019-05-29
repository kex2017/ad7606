/*
 * data_processor.h
 *
 *  Created on: Apr 16, 2019
 *      Author: xuke
 */

#ifndef INTERNAL_AD_SAMPLE_H_
#define INTERNAL_AD_SAMPLE_H_
#include "thread.h"


typedef enum
{
    LINEA = 0,
    LINEB = 1,
    LINEC = 2
} line_temperature_t;

#define SAMPLE_COUNT 64
#define CHANNEL_COUNT 2
#define SAMPLE_DURATION_MS 20
#define PF_CHANNEL_COUNT 2

#define TEMP_ADC2_CHAN_CNT 2
#define TEMP_BAT_ADC3_CHAN_CNT 2

typedef struct pf_over_current_cal_k_b{
    float k;
    float b;
}pf_cal_k_b_t;

typedef struct _threshold_chanagerate
{
    uint16_t pf_threshold;
    uint16_t pf_chanagerate;
    pf_cal_k_b_t pf_cal_k_b;
} PF_THRESHOLD_CHANAGERATE;

typedef struct _pf_data_t
{
    PF_THRESHOLD_CHANAGERATE pf_threshold_chanagerate[CHANNEL_COUNT];
} PF_DATA;

typedef struct _raw_data_t
{
    uint16_t data[CHANNEL_COUNT * SAMPLE_COUNT];
} RAW_DATA;

typedef struct _wave_data
{
    uint16_t data[CHANNEL_COUNT * SAMPLE_COUNT];
    uint32_t time;
} WAVE_DATA;

typedef struct _mutation_data
{
    WAVE_DATA wd;
    uint16_t channel1[SAMPLE_COUNT];
    uint16_t channel2[SAMPLE_COUNT];
    float rms_data[PF_CHANNEL_COUNT];
} MUTATION_DATA;

typedef struct _periodic_data {
   uint16_t channel1[SAMPLE_COUNT];
   uint16_t channel2[SAMPLE_COUNT];
   float rms_data[PF_CHANNEL_COUNT];
} PERIODIC_DATA;

typedef struct _general_call_data {
   uint16_t channel1[SAMPLE_COUNT];
   uint16_t channel2[SAMPLE_COUNT];
   float rms_data[PF_CHANNEL_COUNT];
} GENERAL_CALL_DATA;


kernel_pid_t internal_ad_sample_serv_init(void);

uint16_t get_line_temp(line_temperature_t line);
void do_receive_pid_hook(kernel_pid_t pid);
void pf_set_over_current_cal_k_b(uint8_t channel, pf_cal_k_b_t pf_cal_k_b);
void pf_data_recv_hook(kernel_pid_t pid);
void pray_periodic_task(void);
uint16_t pf_get_threshold(uint8_t channel);
uint16_t pf_get_changerate(uint8_t channel);
pf_cal_k_b_t get_pf_over_current_cal_k_b(uint8_t channel);
uint16_t get_bat_vol(void);
int pf_set_threshold_changerate(uint8_t channel, uint16_t threshold,uint16_t changerate);
void send_mutation_msg_is_done(void);
#endif /* SRC_VC_TEMP_BAT_VOL_SAMPLE_H_ */
