#ifndef FRAME_ENCODE_H_
#define FRAME_ENCODE_H_

#include "frame_common.h"

uint16_t frame_time_ctrl_data_encode(uint8_t *data, uint8_t errorcode, uint32_t timestamp);
uint16_t frame_get_running_state_encode(uint8_t *data, uint8_t errorcode, uint32_t temp, uint32_t humidity, uint32_t voltage, uint8_t work , uint32_t timestamp );
uint16_t frame_set_calibration_info_encode(uint8_t *data, uint8_t errorcode,uint32_t timestamp );
uint16_t frame_get_calibration_info_encode(uint8_t *data, uint8_t errorcode, calibration_info_t *calibration_info);
uint16_t frame_channel_info_encode(uint8_t *data, uint8_t errorcode, channel_info_t* channel_info);
uint16_t frame_set_channel_info_rsp_encode(uint8_t *data, uint8_t errcode, uint32_t timestamp);
uint16_t frame_heart_beat_encode(uint8_t *data, uint8_t errcode, uint32_t timestamp);

/*周期数据编码*/
uint16_t current_cycle_data_encode(uint8_t *data, uint8_t errorcode, float ch1_current, float ch2_current, uint32_t timestamp);
uint16_t high_current_cycle_data_encode(uint8_t *data, uint8_t errorcode, float ch1_current, float ch2_current, uint32_t timestamp);

/*突变数据编码*/
uint16_t high_current_mutation_data_encode(uint8_t * data,  uint8_t errorcode, uint32_t timestamp, uint8_t channel, uint16_t pkg_sum, uint16_t pkg_index, uint8_t * cur_data, uint16_t len );
uint16_t current_mutation_data_encode(uint8_t * data,  uint8_t errorcode, uint32_t timestamp, uint8_t channel, uint16_t pkg_sum, uint16_t pkg_index, uint8_t * cur_data, uint16_t len );


#endif /* FRAME_ENCODE_H_ */
