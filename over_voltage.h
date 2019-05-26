/*
 * @brief       Over voltage event module.
 *              Basic event loop for overvoltage event detect, curve data read
 *              and provide query interface
 *
 */


#ifndef OVER_VOLTAGE_H_
#define OVER_VOLTAGE_H_


#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OVER_VOLTAGE_CHANNEL_COUNT 3


typedef struct _over_voltage_event_info {
    uint16_t happened_flag;
    uint16_t max_value;
    uint16_t sn;
    uint32_t timestamp;
    float k;
}over_voltage_event_info_t;

void clear_channel_over_voltage_happened_flag(uint8_t channel);
void set_over_voltage_event_info(void);


void ov_event_info_channels_init(void);
void over_voltage_process_one_event(uint32_t channels, uint32_t timestamp);


/**
 * @brief   Get channels which has over voltage event happen.
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_voltage_get_event_happened_channels(uint8_t *channles, uint8_t *count);


/**
 * @brief   Get over voltage event details info of given channel.
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_voltage_get_event_info(uint8_t channel, over_voltage_event_info_t *event);

/**
 * @brief   Set  over voltage threshold of give channel
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_voltage_set_threshold(uint8_t channel, uint16_t threshold);
int over_voltage_set_changerate(uint8_t channel, uint16_t changerate);
/**
 * @brief   Get over voltage curve data length of given timestamp;
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
uint16_t over_voltage_get_curve_data_len(uint8_t channel,  uint32_t timestamp, uint32_t *length);

/**
 * @brief   Get over voltage curve data of given timestamp, sn, cur_pkt_num;
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_voltage_get_curve_data(uint8_t channel,  uint32_t timestamp, uint16_t sn, uint16_t cur_pkt_num, uint16_t* total_pkt_count, uint8_t *data, uint16_t pkt_size);

#ifdef __cplusplus
}
#endif

#endif /* OVER_VOLTAGE_H_ */
