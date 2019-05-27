#ifndef FRAME_ENCODE_H_
#define FRAME_ENCODE_H_

#include "frame_common.h"

uint16_t frame_time_ctrl_data_encode(uint8_t *data, uint8_t errorcode, uint32_t timestamp);
uint16_t frame_get_running_state_encode(uint8_t *data, uint8_t errorcode, uint32_t temp, uint32_t humidity, uint32_t voltage, uint8_t work , uint32_t timestamp );
#endif /* FRAME_ENCODE_H_ */
