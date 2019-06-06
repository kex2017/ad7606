#ifndef OVER_CURRENT_H_
#define OVER_CURRENT_H_


#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OVER_CURRENT_CHANNEL_COUNT 2
#define MAX_FPGA_DATA_LEN (4096 * 2) //4k*2  4096

typedef struct _over_current_data{
    uint32_t curve_len;
    uint16_t curve_data[MAX_FPGA_DATA_LEN];
    uint32_t ns_cnt;
    uint32_t one_sec_clk_cnt;
}over_current_data_t;

typedef struct _over_current_info {
    uint16_t threshold;
    uint16_t change_rate;
}over_current_info_t;

kernel_pid_t over_current_service_init(void);

over_current_info_t *get_over_current_info(uint8_t channel);

void set_over_current_threshold(uint8_t channel, uint16_t threshold);
void set_over_current_changerate(uint8_t channel, uint16_t changerate);

void trigger_sample_over_current_by_hand(void);

over_current_data_t *get_over_current_data(void);
int get_over_current_max(uint8_t channel);
int get_over_current_avr(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif
