#ifndef OVER_CURRENT_H_
#define OVER_CURRENT_H_


#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OVER_CURRENT_CHANNEL_COUNT 2
#define MAX_FPGA_DATA_LEN (4096) //4k*2  4096

#define OVER_CURRENT_PRIO              (6)

typedef struct _over_current_data{
    uint32_t curve_len;
    uint16_t curve_data[MAX_FPGA_DATA_LEN];
    uint32_t ns_cnt;
}over_current_data_t;

typedef struct _over_current_cal_k_b{
    float k;
    float b;
}cal_k_b_t;

typedef struct _over_current_info {
    uint16_t threshold;
    uint16_t change_rate;
    cal_k_b_t cal_k_b;
}over_current_info_t;

kernel_pid_t over_current_service_init(void);

over_current_info_t *get_over_current_info(uint8_t channel);

void set_over_current_threshold(uint8_t channel, uint16_t threshold);
void set_over_current_changerate(uint8_t channel, uint16_t changerate);

cal_k_b_t get_over_current_cal_k_b(uint8_t channel);
void set_over_current_cal_k_b(uint8_t channel, cal_k_b_t cal_k_b);


over_current_data_t *get_over_current_data(void);
int get_over_current_max(uint8_t channel);
int get_over_current_avr(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif
