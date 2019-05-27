#ifndef FRAME_ENCODE_H_
#define FRAME_ENCODE_H_

#include "frame_common.h"
#include "dev_cfg.h"

//uint16_t frame_data_encode(uint8_t* in, frame_header_t frame_header, uint8_t* out);
uint16_t frame_heart_beat_rsp_encode(uint8_t *data, uint32_t timestamp, uint8_t * cmd_id);
uint16_t frame_device_config_rsp_encode(uint8_t *data,product_info_t* dev_info, uint8_t* cmd_id);
uint16_t frame_set_or_select_time_rsp_encode(uint8_t * data,uint8_t cmd_statu,  uint32_t timestamp,uint8_t* cmd_id);
uint16_t frame_net_dev_rsp_encode(uint8_t *data, uint8_t* ip_addr, uint8_t* net_mask, uint8_t*  gateway, uint8_t* dns, uint8_t flag,uint8_t* cmd_id);
uint16_t frame_dev_cfg_encode(uint8_t * data, uint8_t flag, uint8_t data_type, uint8_t msg_type, uint8_t * cmd_id);
uint16_t frame_threshold_cfg_encode(uint8_t* data, float threshold, uint8_t type, uint8_t * describe, uint8_t flag, uint8_t sum,uint8_t *cmd_id);
uint16_t frame_set_server_info_encode(uint8_t *data, uint8_t flag, uint8_t *ip,uint16_t port, uint8_t* domain,uint8_t * cmd_id );
uint16_t frame_select_server_info_encode(uint8_t *data, uint8_t flag,server_info_t * server_info, uint8_t * cmd_id);
uint16_t frame_component_dev_encode(uint8_t *data, uint8_t flag,uint8_t * component_id,uint16_t original_id, uint8_t * cmd_id);
uint16_t frame_requset_encode(uint8_t * data, uint8_t flag, uint8_t requset_type,uint8_t * cmd_id);
uint16_t frame_miss_pack_info_encode(uint8_t *data, uint8_t *cmd_id, uint8_t *file_name);
uint16_t device_fault_msg_encode(uint8_t * data,uint32_t fault_time,uint8_t *  fault_des,uint16_t des_len, uint8_t * cmd_id);
uint16_t frame_dev_ip_addr_encode(uint8_t* data, uint8_t flag, uint8_t * p_num, uint8_t* ip_addr,uint8_t * cmd_id );
uint16_t frame_work_condition_rsp_encode(uint8_t * data, uint8_t *cmd_id,float voltage,float temp,float el_quantity ,uint8_t state,float sum_time,float work_time,uint8_t net_state,uint8_t csq);

uint16_t frame_dev_cur_encode(uint8_t * data, mx_sensor_info_t* p);
uint16_t frame_dev_alarm_encode(uint8_t * data, st_sensor_info_t *p);
uint16_t frame_dev_sc_alarm_encode(uint8_t * data, sc_sensor_info_t *p);
uint16_t frame_sample_ctrl_rsp_encode(uint8_t * data, uint8_t command_status, sample_param_t* p_sample, uint8_t * cmd_id);
uint16_t frame_dev_reset_rsp_encode(uint8_t* data, uint8_t command_status, uint8_t * cmd_id);
uint16_t frame_dev_vol_encode(uint8_t * data, mx_sensor_info_t* p);
uint16_t frame_dev_hfct_encode(uint8_t * data, mx_sensor_info_t* p);
uint16_t frame_temperature_encode(uint8_t *data, mx_sensor_info_t* p);
#endif /* FRAME_ENCODE_H_ */
