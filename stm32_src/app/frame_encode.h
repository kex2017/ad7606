#ifndef FRAME_ENCODE_H_
#define FRAME_ENCODE_H_

#include "frame_common.h"

uint16_t frame_time_ctrl_data_encode(uint8_t *data, uint8_t errorcode,uint8_t type, uint32_t timestamp);
uint16_t frame_get_running_state_encode(uint8_t *data, uint8_t errorcode, uint32_t temp, uint32_t humidity, uint32_t voltage, uint8_t work , uint32_t timestamp );
uint16_t frame_reboot_encode(uint8_t *data, uint8_t errorcode,uint32_t timestamp );
uint16_t frame_channel_info_encode(uint8_t *data, uint8_t errorcode, channel_info_t* channel_info);
uint16_t frame_set_channel_info_rsp_encode(uint8_t *data, uint8_t errcode,uint8_t channel, float k, float b);
uint16_t frame_heart_beat_encode(uint8_t *data, uint8_t errcode, uint32_t timestamp);
uint16_t frame_transfer_file_rsp_encode(uint8_t *data, uint8_t errcode, uint8_t file_type, uint16_t cur_packet);
uint16_t dev_info_encode(uint8_t *data, uint8_t errorcode, uint8_t * version, uint16_t version_len, double longitude, double latitude, double altitude);
uint16_t frame_collection_cycle_data_encode(uint8_t *data, uint8_t errcode, uint8_t type, uint16_t cycle);

/*周期数据编码*/
uint16_t current_cycle_data_encode(uint8_t *data, uint8_t errorcode, uint8_t send_data_type,  uint8_t channel_count ,uint8_t channel_1, uint32_t ch1_current, uint8_t channel_2,uint32_t ch2_current, uint32_t timestamp);

/*突变数据编码*/
uint16_t current_mutation_data_encode(uint8_t * data,  uint8_t errorcode, uint8_t send_type,uint32_t timestamp, uint32_t ns_clk_num, uint8_t channel, uint16_t pkg_sum, uint16_t pkg_index, uint8_t * cur_data, uint16_t len );

#endif /* FRAME_ENCODE_H_ */
