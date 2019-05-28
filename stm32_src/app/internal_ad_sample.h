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

#define TEMP_ADC2_CHAN_CNT 2
#define TEMP_BAT_ADC3_CHAN_CNT 2

#define PF_OVER_LIMIT 2048

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
    uint32_t channel1[SAMPLE_COUNT];
    uint32_t channel2[SAMPLE_COUNT];
} MUTATION_DATA;

typedef struct _periodic_data {
   uint32_t channel1[SAMPLE_COUNT];
   uint32_t channel2[SAMPLE_COUNT];
} PERIODIC_DATA;

typedef enum{
   MUTATION_DATA_TYPE = 1,
   GENERAL_CALL_DATA_TYPE,
   PERIODIC_DATA_TYPE,
   MOCK_IGNORE_DATA_TYPE,
}MSG_TYPE;

kernel_pid_t pf_sample_serv_init(void);

uint16_t get_line_temp(line_temperature_t line);
void do_receive_pid_hook(kernel_pid_t pid);
void set_periodic_task_thread_init(void);
uint16_t get_bat_vol(void);

#endif /* SRC_VC_TEMP_BAT_VOL_SAMPLE_H_ */