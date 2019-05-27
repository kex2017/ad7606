#ifndef OVER_CURRENT_H_
#define OVER_CURRENT_H_


#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OVER_CURRENT_CHANNEL_COUNT 2

/**
 * @brief   Priority of the over current thread
 */
#ifndef OVER_CURRENT_PRIO
#define OVER_CURRENT_PRIO              (6)
#endif

/**
 * @brief   Stack size used for the over current thread
 */
#ifndef OVER_CURRENT_STACKSIZE
#define OVER_CURRENT_STACKSIZE          (THREAD_STACKSIZE_MAIN)
#endif



typedef struct _over_current_event_info {
    uint16_t happened_flag;
    uint16_t max_value;
    uint16_t avr_value;
    uint16_t sn;
    uint32_t timestamp;
    float k;
}over_current_event_info_t;

void clear_channel_over_current_happened_flag(uint8_t channel);
void set_over_current_event_info(void);


/**
 * @brief   The PID of the over current thread
 */
extern kernel_pid_t over_current_pid;

/**
 * @brief   Start the over current detected thread
 *
 * @return  PID of the over current thread
 * @return  negative value on error
 */
kernel_pid_t over_current_service_init(void);

/**
 * @brief   Notify that over current event happen.
 *          Usually called by hardware PIN interrupt.
 *
 * @return  negative value on error;
 *          FPGA not ready etc...

 */
int over_current_event_happen_notify(void);

/**
 * @brief   Get channels which has over current event happen.
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_current_get_event_happened_channels(uint8_t *channles, uint8_t *count);


/**
 * @brief   Get over current event details info of given channel.
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_current_get_event_info(uint8_t channel, over_current_event_info_t *event);

/**
 * @brief   Set  over current threshold of give channel
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 */
int over_current_set_threshold(uint8_t channel, uint16_t threshold);
int over_current_set_changerate(uint8_t channel, uint16_t changerate);
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
