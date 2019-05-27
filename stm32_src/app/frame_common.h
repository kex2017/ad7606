#ifndef FRAME_COMMON_H_
#define FRAME_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "log.h"
#include "frame_decode.h"

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

typedef struct _header {
    uint8_t starter1;
    uint16_t slave_addr;
    uint16_t master_addr;
    uint16_t data_len;
} frame_header_t;


typedef struct _frame_req {
	frame_header_t header;
    uint8_t func_code;
   union {
   } frame_req;
   uint8_t cs;
}frame_req_t;


uint8_t byte_sum_checksum(uint8_t *data, uint32_t length);


#endif /* FRAME_COMMON_H_ */
