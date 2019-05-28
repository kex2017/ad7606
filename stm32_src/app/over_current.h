#ifndef OVER_CURRENT_H_
#define OVER_CURRENT_H_


#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OVER_CURRENT_CHANNEL_COUNT 2

#define OVER_CURRENT_PRIO              (6)

typedef struct _over_current_cal_k_b{
    float k;
    float b;
}cal_k_b_t;

typedef struct _over_current_info {
    uint16_t max_value;
    uint16_t avr_value;
    uint32_t timestamp;
    uint16_t threshold;
    uint16_t change_rate;
    cal_k_b_t cal_k_b;
}over_current_info_t;

void clear_channel_over_current_happened_flag(uint8_t channel);
void set_over_current_event_info(void);

kernel_pid_t over_current_service_init(void);


int over_current_get_event_info(uint8_t channel, over_current_info_t *event);

void set_over_current_threshold(uint8_t channel, uint16_t threshold);
void set_over_current_changerate(uint8_t channel, uint16_t changerate);
/**
 * @brief   Get over current curve data length of given timestamp;
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
uint16_t over_current_get_curve_data_len(uint8_t channel,  uint32_t timestamp, uint32_t *length);

/**
 * @brief   Get over current curve data of given timestamp, sn, cur_pkt_num;
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_current_get_curve_data(uint8_t channel,  uint32_t timestamp, uint16_t sn, uint16_t cur_pkt_num, uint16_t* total_pkt_count, uint8_t *data, uint16_t pkt_size);

#ifdef __cplusplus
}
#endif

#endif
