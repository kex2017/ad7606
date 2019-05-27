#ifndef FRAME_COMMON_H_
#define FRAME_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "frame_decode.h"

#define FRAME_STARTER_H  0xa5
#define FRAME_STARTER_L  0x5a
#define SET_ERROR (0x00)
#define SET_SUCCESS (0xFF)
#define SPARE (0x00)
#define FRAME_HEADER_MAX_LEN 24
#define FRAME_CS_LEN 0X02

#define FRAME_DATA_LEN_INDEX 0x02
#define FRAME_TYPE_INDEX 21

typedef enum  {
    TEMP_CTRL_REQ_TYPE = 0XBA,
    CURRENT_CTRL_REQ_TYPE = 0xBC,
    SC_ALARM_CTRL_REQ_TYPE = 0xC2,
    HFCT_CTRL_REQ_TYPE = 0xC4,
    ALARM_CTRL_REQ_TYPE = 0xC5,
    VOLTAGE_CTRL_REQ_TYPE = 0xC7
} ctrl_req_t ;

#define RESET_ARM_TYPE (0x0000)

#define MAX_REQ_DATA_LEN 200
#define MAX_RSP_DATA_INFO_LEN 32
#define MAX_RSP_DATA_LEN 250
#define BUFFER_LEN 1024
#define MAX_REQ_FRAME_LEN (FRAME_HEADER_MAX_LEN + MAX_REQ_DATA_LEN + FRAME_CS_LEN)
#define MAX_RSP_FRAME_LEN (FRAME_HEADER_MAX_LEN + MAX_RSP_DATA_INFO_LEN + MAX_RSP_DATA_LEN + FRAME_CS_LEN)

#define PACKET_SYNC (0x5AA5)
#define CMD_ID_LEN  (17)

#define DECODE_CTR_DATA (0)
#define DECODE_RSP_DATA (1)
#define FRAME_DECODE_HEAED_LEN 21

#define TIMESTAMP_DATA_LEN 0x4
#define DEVICE_CONFIG_DATA_LEN 0xa8
#define WORK_CONDITION_DATA_LEN 0x38
#define TIME_CTRL_DATA_LEN 0x05
#define NET_DEV_CTRL_DATA_LEN 0x21
#define DEV_CFG_SEND_DATA_LEN 0x03
#define THRESHOLD_RSP_DATA_LEN 0x03
#define SERVER_INFO_DATA_LEN 0x53
#define COMPONENT_DEV_DATA_LEN 0x14
#define REQUSET_RSP_DATA_LEN 0x02
#define UPGRADE_MISS_PACK_DATA_LEN 0x18
#define GET_DEV_IP_DATA_LEN 0x19

#define QUERY_CMD (0x00)
#define SET_CMD (0x01)

#define SEND_SUCCESS (0xFF)
#define SEND_FAILURE (0x00)

#define FRAME_HEADER_COMMON_LEN (23)

#define CTRL_TIME_PKT 0xA1               //6.3
#define CTRL_DEV_NET_INFO_PKT 0xA2
#define SUPERIOR_GET_DATA_PACK_TYPE 0xA3
#define CTRL_SAMPLE_INFO_PKT 0xA4
#define MODEL_CONFIG_SET_PACK_TYPE 0xA5
#define CTRL_THRESHOLD_PKT 0xA6
#define CTRL_SERVER_NET_INFO_PKT 0xA7
#define CTRL_DEV_CFG_PKT 0xA8
#define CTRL_DEV_UPGRADE_TRANSFER_PKT 0xA9
#define CTRL_DEV_UPGRADE_CNF_PKT 0xAA
#define CTRL_UPGRADE_MISS_FILE_PKT 0xAB
#define COMPONENT_DEV_PACK_TYPE 0xAC
#define SET_REVIVAL_TIME_PAKC_TYPE 0xAE
#define CTRL_DEV_IP_PACK_TYPE 0x9A


#define HEART_BEAT_PACK_TYPE 0xE6
#define DEV_CONFIG_PACK_TYPE 0xE7
#define WORK_CONDITION_PACK_TYPE 0xE8
#define FAULT_MSG_PACK_TYPE 0xE9

typedef enum {
    CUR_DATA_UPLOAD_PKT = 0x29,
    HFCT_DATA_UPLOAD_PKT = 0X2A,
    VOL_DATA_UPLOAD_PKT = 0X2B,
    TEMP_DATA_UPLOAD_PKT = 0X2C,
    SHORT_CIRCUIT_ALARM_UPLOAD_PKT = 0X84,
    ALARM_STATUS_UPLOAD_PKT = 0X87,
}monitor_data_upload_pkt;

#define CTRL_DEV_RESET_PKT 0xAD
#define OUT_CABLE_VOLTAGE_MONITOR_PACK_TYPE 0X2B
#define PARTIAL_DISCHARGE_DETECTION_APCK_TYPE 0X2A
#define TEST_TEMPERATURE_PACK_TYPE 0X2C

#define SET_REVIVAL_TIME_PAKC_TYPE 0xAE

#define MONITOR_DATA_FRAME (0x01) //监测数据报(监测装置 -> 上位机)
#define MONITOR_RSP_FRAME  (0x02) //数据响应报(上位机 -> 监测装置)
#define CONTROL_DATA_FRAME (0x03) //控制数据报(上位机 -> 监测装置)
#define CONTROL_RSP_FRAME  (0x04) //控制响应报(监测装置 -> 上位机)
#define WORK_CONDITION_RSP_FRAME (0x07)
#define WORK_CONDITIN_REQ_FRAME (0x08)

typedef struct _frame_header {
    uint16_t sync;              //报文头：2Byte
    uint16_t packet_length;     //报文长度：2Byte
    uint8_t cmd_id[17];         //状态监测装置ID：17Byte
    uint8_t frame_type;         //帧类型：1Byte
    uint8_t packet_type;        //报文类型：1Byte
    uint8_t request_set_flag;   //指令状态：1Byte
}frame_header_t;

typedef struct _set_device_time{
	uint8_t type;
    uint32_t time;
}set_device_time_t;

typedef struct _net_dev{
	uint8_t type;
	uint8_t ip_addr[4];
	uint8_t net_mask[4];
	uint8_t gateway[4];
	uint8_t dns[4];
}net_dev_t;

typedef struct _threshold_data_t{
	uint8_t warn_cfg[6];
	uint32_t threshold;
}threshold_data_t;

typedef struct _threshold_t
{
	uint8_t req_type;
	uint8_t data_type;
    uint8_t sum;
    threshold_data_t thr_data[20];
}threshold_t;

typedef struct _dev_cfg_select_t
{
	uint8_t req_type;
	uint8_t data_type;
	uint8_t msg_type;
}dev_cfg_select_t;

typedef struct _server_cfg_select_t
{
	uint8_t req_type;
	uint8_t ip[4];
	uint16_t port;
	uint8_t domain[64];
}server_cfg_select_t;

typedef struct _revival_time_t
{
	uint32_t ref_rev_time;
	uint16_t rev_crcle; 
	uint16_t d_time;
	uint32_t reserve ;
}revival_time_t;

typedef struct _sample_param_t
{
	uint8_t request_set_flag;
	uint8_t request_type;
	uint16_t main_time;
	uint16_t sample_count;
	uint16_t sample_frequency;
	uint32_t reserve ;
}sample_param_t;

typedef struct _cable_c_v_monitor_t
{
    uint8_t  cmd_id[17]; 
	uint32_t time_stamp;
	uint16_t  alerm_flag;
	uint32_t max_validity ;
	uint32_t min_validity ;
	uint32_t validity ;
	uint32_t reserve1 ;
	uint32_t reserve2 ;
}cable_c_v_monitor_t;

typedef struct _cable_partial_discharge_t
{
    uint8_t  cmd_id[17]; 
	uint32_t time_stamp;
	uint16_t alerm_flag;
	uint32_t amplitude ;
	uint16_t frequency;
	uint32_t reserve1 ;
	uint32_t reserve2 ;
}cable_partial_discharge_t;

typedef struct _sample_data_t
{
	uint32_t position;
	float    temperature;
}sample_data_t;

typedef struct _cable_test_temperature_t
{
    uint8_t  cmd_id[17]; 
	uint32_t time_stamp;
	uint16_t alerm_flag;
	uint16_t sampleNum;
	sample_data_t sample_data[6];
	uint32_t reserve1 ;
	uint32_t reserve2 ;
}cable_test_temperature_t;

typedef struct _component_dev_t{
	uint8_t req_type;
	uint8_t id[17];
	uint16_t original_id ;
}component_dev_t;

typedef struct _request_data_t
{
	uint8_t request_type;
	uint32_t start_time;
	uint32_t end_time;
}request_data_t;

typedef struct _dev_register_t
{
	uint8_t msg_send_flag;
	uint8_t register_flag;
}dev_register_t;

typedef struct _transfer_file_req_t {
    uint8_t file_type;
    uint32_t file_count;
    uint32_t file_index;
    uint8_t file_name[20];
    uint16_t data_len;
    char buff[BUFFER_LEN];
} transfer_file_req_t;

typedef struct _dev_ip_t
{
	uint8_t req_type;
	uint8_t no[16];
}dev_ip_t;

typedef struct _frame_req {
   frame_header_t header;
   uint8_t frame_type;
   uint8_t msg_type;
   union {
	uint8_t   w_commstatus;
	uint16_t  dw_commstatus;
       set_device_time_t device_time;
       net_dev_t net_dev;
       threshold_t threshold_cfg;
       dev_cfg_select_t dev_cfg_select;
       server_cfg_select_t server_cfg;
       component_dev_t component_dev;
       request_data_t request_data;
       sample_param_t  sample_param; 
      dev_register_t dev_register;
	   revival_time_t  revival_time;
	   transfer_file_req_t transfer_file_req;
	   dev_ip_t dev_ip;
   } frame_req;
   uint16_t cs;
}frame_req_t;


unsigned short RTU_CRC(unsigned char*puchMsg, unsigned short usDataLen);

uint16_t frame_encode(uint8_t* in, frame_header_t frame_header, uint8_t* out);

uint16_t frame_set_device_time_rsp_encode(uint8_t cmd_status, uint32_t cur_timestamp, uint8_t* out);

uint16_t frame_decode(uint8_t *frame_data, frame_req_t *master_frame_req);

uint16_t frame_set_time_req_decode(uint8_t *data, frame_req_t *req);

void test_encode_decode(void);

#endif /* FRAME_COMMON_H_ */
