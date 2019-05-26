#ifndef FRAME_ENCODE_H_
#define FRAME_ENCODE_H_

#include "frame_common.h"
#include "curve_harmonics.h"

uint16_t frame_double_encode(uint8_t *data,double value);
uint16_t frame_uint8_encode(uint8_t *data,uint8_t value);
uint16_t frame_uint32_encode(uint8_t *data,uint32_t value);

uint16_t frame_sample_rsp_encode(uint8_t *data, uint8_t errcode, uint8_t channel_count, uint32_t timestamp);
uint16_t frame_hf_current_jf_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp);
uint16_t frame_dielectric_loss_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp);
uint16_t frame_pf_current_encode(uint8_t *data, float *value);
uint16_t frame_pf_current_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode, uint8_t channel, uint32_t timestamp);
uint16_t frame_phase_hamonics_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode, uint8_t channel, uint32_t timestamp);
uint16_t frame_collector_info_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode, uint8_t channel, uint32_t timestamp);
uint16_t frame_over_voltage_event_info_encode(uint8_t *data, uint8_t channel, uint32_t timestamp, uint16_t sn, float max_value);
uint16_t frame_over_voltage_search_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode);
uint16_t frame_over_voltage_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp, uint16_t sn);
uint16_t frame_transfer_file_rsp_encode(uint8_t *data, uint8_t errcode, uint8_t file_type, uint16_t cur_packet);
uint16_t frame_heart_beat_encode(uint8_t *data, uint8_t errcode, uint32_t timestamp);
uint16_t frame_pd_threshold_rsp_encode(uint8_t *data, uint8_t errcode);
uint16_t frame_search_pd_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode);
uint16_t frame_partial_discharge_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp);
uint16_t frame_prpd_data_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t start_timestamp, uint32_t end_timestamp);
uint16_t frame_set_current_time_rsp_encode(uint8_t *data, uint8_t errcode);
uint16_t frame_send_version_rsp_encode(uint8_t *data, uint8_t* version,uint16_t len,uint8_t errcode);
uint16_t frame_reboot_rsp_encode(uint8_t *data, uint8_t errcode);
uint16_t frame_current_phase_harmonics_data_encode(uint8_t *data,uint8_t errcode,channel_harmonics_info_t * harmonics_info,uint32_t timestamp);
uint16_t frame_phase_harmonics_encode(uint8_t *data, float *value);
uint16_t frame_send_temperature_rsp_encode(uint8_t *data, float *temperature, uint32_t timestamp, uint8_t errcode);
int get_version_info(uint8_t * version);
#endif
