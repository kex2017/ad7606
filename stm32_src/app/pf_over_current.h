#ifndef PF_OVER_CURRENT_H_
#define PF_OVER_CURRENT_H_


#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PF_OVER_CURRENT_CHANNEL_COUNT 2
#define PF_CHANNEL_OFFSET 2


#define MAX_PF_FPGA_DATA_LEN (3072 * 2) //4k*2  4096

typedef struct _pf_over_current_data{
    uint32_t curve_len;
    uint16_t curve_data[MAX_PF_FPGA_DATA_LEN/2];
    uint32_t ns_cnt;
    uint32_t one_sec_clk_cnt;
    uint32_t timestamp;
    uint8_t phase;
}pf_over_current_data_t;

typedef struct _pf_over_current_info {
    uint16_t threshold;
}pf_over_current_info_t;

kernel_pid_t pf_over_current_service_init(void);

pf_over_current_info_t *get_pf_over_current_info(uint8_t channel);

void set_pf_over_current_threshold(uint8_t channel, uint16_t threshold);

void trigger_sample_pf_over_current_by_hand(void);

pf_over_current_data_t *get_pf_over_current_data(void);
float get_pf_over_current_rms(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif
