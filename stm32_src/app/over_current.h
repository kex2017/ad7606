#ifndef OVER_CURRENT_H_
#define OVER_CURRENT_H_


#include <stdint.h>
#include "kernel_types.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_DEBUG (0)

#define MAX_HF_OVER_CURRENT_CHANNEL_COUNT 2

#define MAX_PF_OVER_CURRENT_CHANNEL_COUNT 2

#define FPGA_PHASE_NUM 3

#define PF_CHAN_OFFSET 2

#define MAX_PF_FPGA_DATA_LEN (3072 * 2) //4k*2  4096

#define HF_TYPE 0
#define PF_TYPE 1

#define OVER_CURRENT_CHANNEL_COUNT 4

#define MAX_FPGA_DATA_LEN (4096 * 2) //4k*2  4096

typedef struct _over_current_data{
    uint32_t curve_len;
    uint32_t ns_cnt;
    uint32_t one_sec_clk_cnt;
    uint32_t timestamp;
    uint8_t phase;
    uint8_t data_type;
}over_current_data_t;

typedef struct _hf_over_current_info {
    uint16_t threshold;
    uint16_t change_rate;
}hf_over_current_info_t;

typedef struct _pf_over_current_info {
    uint16_t threshold;
}pf_over_current_info_t;

typedef struct _send_curve_info{
    uint8_t send_type;
    uint8_t phase;
    uint8_t channel;
}send_curve_info_t;

uint16_t get_fpga_uint16_data(uint16_t data);

float get_pf_rms(uint8_t phase, uint8_t channel);

uint32_t get_hf_max(uint8_t phase, uint8_t channel);

kernel_pid_t hf_pf_over_current_service_init(void);

hf_over_current_info_t *get_hf_over_current_info(uint8_t phase, uint8_t channel);

void set_hf_over_current_threshold(uint8_t phase, uint8_t channel, uint16_t threshold);
void set_hf_over_current_changerate(uint8_t phase, uint8_t channel, uint16_t changerate);
void set_pf_over_current_threshold(uint8_t phase, uint8_t channel, uint16_t threshold);
void trigger_sample_over_current_by_hand(void);

over_current_data_t *get_over_current_data(uint8_t phase, uint8_t channel);
int get_hf_over_current_max(uint8_t channel);
int get_hf_over_current_avr(uint8_t channel);

float get_pf_over_current_rms(uint8_t channel);

void over_current_hook(kernel_pid_t pid);

#ifdef __cplusplus
}
#endif

#endif
