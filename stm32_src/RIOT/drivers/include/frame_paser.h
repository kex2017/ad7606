#ifndef _FRAME_PASER_H
#define _FRAME_PASER_H

#include "ringbuffer.h"
#include "periph/uart.h"
#include "mutex.h"

#define FRAME_PASER_MAX_LEN 1024
typedef int (*frame_paser_crc_cb_t)(uint8_t* data,uint16_t data_len,uint8_t *crc_buf,uint16_t crc_len);


typedef struct {

	ringbuffer_t rx_buf;
	char *rx_mem;
	uint16_t rx_mem_size;

	uint8_t *header_list;
	uint16_t header_num;

	uint16_t frame_min_len;
	uint16_t frame_max_len;
	uint16_t frame_info_len;

	uint16_t check_len;
	uint32_t parsed_frame_count;
	uint32_t error_framer_count;

	frame_paser_crc_cb_t frame_paser_crc_cb;

}frame_paser_dev_t;

int frame_paser_init(frame_paser_dev_t * dev, char *rx_mem,uint16_t rx_mem_size,
		uint8_t *header_list,uint16_t header_num, frame_paser_crc_cb_t cb, uint16_t check_len,uint16_t frame_info_len);

int is_frame_parser_busy(frame_paser_dev_t* dev);
void add_frame_data_stream(frame_paser_dev_t* dev,uint8_t *buf,uint16_t buf_len);
int do_frame_parser(frame_paser_dev_t* dev,uint8_t *buf,uint16_t buf_len);
uint32_t frame_paser_get_parsed_frame_count(frame_paser_dev_t *dev);

uint32_t frame_paser_get_error_frame_count(frame_paser_dev_t *dev);

#endif
