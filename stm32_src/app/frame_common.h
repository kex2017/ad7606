#ifndef FRAME_COMMON_H_
#define FRAME_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "log.h"

#define FRAME_STARTER  0xDB
#define MASTER_ADDR 0X1001

#define FRAME_HEADER_LEN 0X07
#define SET_ERROR (0x00)
#define SET_SUCCESS (0xFF)
#define SPARE (0x00)
#define FRAME_CS_LEN 0X02
#define FRAME_HEADER_MAX_LEN 1

#define FRAME_DATA_LEN_INDEX 0x02
#define FRAME_TYPE_INDEX 21

#define MAX_REQ_DATA_LEN 200
#define MAX_RSP_DATA_INFO_LEN 32
#define MAX_RSP_DATA_LEN 250
#define BUFFER_LEN 1024
#define MAX_REQ_FRAME_LEN (FRAME_HEADER_MAX_LEN + MAX_REQ_DATA_LEN + FRAME_CS_LEN)
#define MAX_RSP_FRAME_LEN (FRAME_HEADER_MAX_LEN + MAX_RSP_DATA_INFO_LEN + MAX_RSP_DATA_LEN + FRAME_CS_LEN)

#define HEART_BEAT_RSP 0xA0
#define HEART_BEAT_RSP_DATA_LEN   0X06


#define TIME_CTRL_REQ 0xA1
#define TIME_CTRL_RSP 0xA2
#define TIME_CTRL_RSP_DATA_LEN 0x07

#define GET_RUNNING_STATE_REQ 0xA3
#define GET_RUNNING_STATE_RSP 0xA4
#define GET_RUNNING_STATE_RSP_DATA_LEN 0x13

#define GET_CHANNEL_INFO_REQ 0xA5
#define GET_CHANNEL_INFO_RSP 0xA6
#define GET_CHANNEL_INFO_RSP_DATA_LEN 0x0B

#define SET_CHANNEL_INFO_REQ 0xA7
#define SET_CHANNEL_INFO_RSP 0xA8
#define SET_CHANNEL_INFO_RSP_DATA_LEN 0x07

#define GET_DEV_INFO_REQ 0xA9
#define GET_DEV_INFO_RSP 0xAA
#define GET_DEV_INFO_RSP_DATA_LEN 0x1A

#define SET_CALIBRATION_INFO_REQ 0xAB
#define SET_CALIBRATION_INFO_RSP 0xAC
#define SET_CALIBRATION_INFO_RSP_DATA_LEN  0x06

#define GET_CALIBRATION_INFO_REQ 0xAD
#define GET_CALIBRATION_INFO_RSP 0xAE
#define GET_CALIBRATION_INFO_RSP_DATA_LEN  0x22

#define DIP_ANGLE_CYCLE_RSP 0xAF
#define DIP_ANGLE_CYCLE_RSP_DATA_LEN 0x12

#define REBOOT_ARM_REQ 0xB0
#define REBOOT_ARM_RSP 0xB1
#define REBOOT_ARM_RSP_DATA_LEN 0x06

#define FRAME_TRANSFER_FILE_REQ   0X68
#define FRAME_TRANSFER_FILE_RSP   0X69
#define FRAME_TRANSFER_FILE_RSP_DATA_LEN   0X05

#define COLLECTION_CYCLE_REQ 0x70
#define COLLECTION_CYCLE_RSP 0x71
#define COLLECTION_CYCLE_RSP_DATA_LEN 0x05

#define SERVER_REQUEST_DATA_REQ 0x50

#define POWER_CURRENT_RSP 0x51
#define POWER_CURRENT_RSP_DATA_LEN 0x08

#define CURRENT_WAVE_FORM_RSP 0x53
#define CURRENT_WAVE_FORM_RSP_DATA_LEN 0x0C


typedef struct _header {
    uint8_t starter1;
    uint16_t slave_addr;
    uint16_t master_addr;
    uint16_t data_len;
} frame_header_t;

typedef struct _time_info{
	uint8_t type;
	uint32_t timestamp;
}time_info_t;

typedef struct _channel_info{
	uint8_t channel;
	uint16_t threshold;
	uint16_t change_rate;
}channel_info_t;

typedef struct _requset_data{
	uint8_t type;
}requset_data_t;

typedef struct _collection_cycle{
	uint8_t type;
	uint16_t cycle;
}collection_cycle_t;

typedef struct _transfer_file_req_t {
    uint8_t file_type;
    uint16_t file_count;
    uint16_t file_index;
    uint8_t md5[32];
    uint16_t data_len;
    char buff[BUFFER_LEN];
} transfer_file_req_t;

typedef struct _frame_req {
	frame_header_t header;
    uint8_t func_code;
   union {
    	time_info_t time_info;
    	channel_info_t channel_info;
        transfer_file_req_t transfer_file_req;
    	requset_data_t requset_data;
    	collection_cycle_t collection_cycle;
   } frame_req;
   uint8_t cs;
}frame_req_t;

uint8_t byte_sum_checksum(uint8_t *data, uint32_t length);

#endif /* FRAME_COMMON_H_ */
