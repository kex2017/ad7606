#include "frame_paser.h"
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "ringbuffer.h"
#include "byteorder.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define PAYLOAD_LEN_OCTET 2
#define CRC_LEN_OCTET 2

//static void frame_paser_dump_hex(uint8_t* data, uint16_t len)
//{
//	if(ENABLE_DEBUG)
//	{
//		int i = 0;
//		DEBUG("HEX: ");
//		for(i=0;i<len;i++)
//		{
//			DEBUG("%02x ",data[i]);
//		}
//		DEBUG("\r\n\r\n");
//	}
//}

int frame_paser_init(frame_paser_dev_t * dev, char *rx_mem,uint16_t rx_mem_size, uint8_t *header_list,uint16_t header_num,frame_paser_crc_cb_t cb, uint16_t check_len, uint16_t frame_info_len )
{
	if (rx_mem_size < header_num + PAYLOAD_LEN_OCTET + CRC_LEN_OCTET) {
		DEBUG("Frame ring buffer is to short");
		return -1;
	}

	if (rx_mem_size >header_num + FRAME_PASER_MAX_LEN) {
		DEBUG("Frame ring buffer is to long ");
		return -1;
	}

	dev->rx_mem = rx_mem;
	dev->rx_mem_size = rx_mem_size;
	dev->header_list = header_list;
	dev->header_num = header_num;

	dev->frame_min_len = header_num+check_len ;
	dev->frame_max_len = rx_mem_size;
	dev->frame_info_len = frame_info_len;

	dev->parsed_frame_count = 0;
	dev->error_framer_count = 0;

	dev->frame_paser_crc_cb =cb ;
	dev->check_len = check_len;

	ringbuffer_init(&(dev->rx_buf), dev->rx_mem, rx_mem_size);

	DEBUG("[FRAME_PARSER]: Frame parser init ok\r\n");
	return 0;
}

static int seek_header_n(frame_paser_dev_t* dev,uint16_t header_index)
{
	if(ringbuffer_avail(&dev->rx_buf) < (unsigned int)(dev->frame_min_len - header_index))
	{
		return -1;
	}

	unsigned int index = 0;

	for(index = 0;index < ringbuffer_avail(&dev->rx_buf) - header_index; index++)
	{
		if(ringbuffer_peek_index(&dev->rx_buf,header_index) == dev->header_list[header_index])
		{
			return 1;
		}
		else
		{
			ringbuffer_remove(&dev->rx_buf,1);
		}
	}
	return -1;
}

static int seek_all_header(frame_paser_dev_t *dev)
{
	int i = 0;
	for(i=0;i<dev->header_num;i++)
	{
		if(seek_header_n(dev,i) < 0)
		{
			return -1;
		}
	}
	return 0;
}

static int seek_payload_len(frame_paser_dev_t* dev)
{
	if(ringbuffer_avail(&dev->rx_buf) < (unsigned int)(dev->frame_min_len - dev->header_num))
	{
		return -1;
	}

	uint16_t payload_len = 0;

	uint16_t payload_len_octet_index = dev->header_num;

	payload_len = ringbuffer_peek_index(&dev->rx_buf,payload_len_octet_index) | ringbuffer_peek_index(&dev->rx_buf,payload_len_octet_index+1) << 8;

	if(payload_len > dev->frame_max_len)
	{
		ringbuffer_remove(&dev->rx_buf,dev->header_num+ PAYLOAD_LEN_OCTET);
		return -1;
	}
	return payload_len+dev->frame_info_len;
}

int is_frame_parser_busy(frame_paser_dev_t* dev)
{
	if(ringbuffer_avail(&dev->rx_buf) > 1024)
	{
		return 1;
	}
	return 0;
}

void add_frame_data_stream(frame_paser_dev_t* dev,uint8_t *buf,uint16_t buf_len)
{
		ringbuffer_add(&dev->rx_buf,(const char*)buf,(unsigned) buf_len);
}

int do_frame_parser(frame_paser_dev_t* dev,uint8_t *buf,uint16_t buf_len)
{
	char temp_buf[FRAME_PASER_MAX_LEN] = { 0 };
	int payload_len = 0;

	if (seek_all_header(dev) < 0) {
		return -1;
	}

	if ((payload_len = seek_payload_len(dev)) < 0) {
		return -1;
	}

	uint16_t total_package_size = 0;
	total_package_size = dev->header_num + PAYLOAD_LEN_OCTET + payload_len
			+ CRC_LEN_OCTET;

	if (ringbuffer_avail(&dev->rx_buf) < total_package_size) {
		return -1;
	}

	ringbuffer_peek(&dev->rx_buf, temp_buf, total_package_size);

	char crc[CRC_LEN_OCTET] ={0};
	dev->frame_paser_crc_cb((uint8_t *)temp_buf,total_package_size - CRC_LEN_OCTET ,(uint8_t *) crc, dev->check_len);
	uint16_t calc_crc = crc[1]  | crc[0] << 8;
	uint16_t recv_crc = 0;
	recv_crc = temp_buf[total_package_size - CRC_LEN_OCTET]<< 8 | temp_buf[total_package_size - CRC_LEN_OCTET+ 1] ;

	if (recv_crc != calc_crc) {
		DEBUG("[frame parser]: crc check error recv %x should be (%x)\r\n\r\n",
				recv_crc, calc_crc);
		dev->error_framer_count++;
		ringbuffer_remove(&dev->rx_buf, dev->header_num + PAYLOAD_LEN_OCTET);
		return -1;
	}

	if (total_package_size > buf_len) {
		return -1;
	}

	memcpy(buf, temp_buf, total_package_size);
	ringbuffer_remove(&dev->rx_buf, total_package_size);
	dev->parsed_frame_count++;

//	frame_paser_dump_hex((uint8_t*) temp_buf, total_package_size);

	return total_package_size;;
}

//int frame_paser_send(frame_paser_dev_t* dev,uint8_t *buf,uint16_t buf_len)
//{
//	if(buf_len > FRAME_PASER_MAX_LEN - dev->header_num - PAYLOAD_LEN_OCTET - CRC_LEN_OCTET )
//	{
//		DEBUG("[uart_frame]: send len (%d) is too long\r\n",buf_len);
//		return -1;
//	}
//
//	char temp_buf[FRAME_PASER_MAX_LEN] = {0};
//	uint16_t payload_len = 0;
//
//	payload_len = htons(buf_len);
//	memcpy(temp_buf,dev->header_list,dev->header_num);
//	memcpy(&temp_buf[dev->header_num],(uint16_t*)(&payload_len),PAYLOAD_LEN_OCTET);
//	memcpy(&temp_buf[dev->header_num + PAYLOAD_LEN_OCTET],buf, buf_len);
//
//	char crc[CRC_LEN_OCTET] ={0};
//	dev->frame_paser_crc_cb((uint8_t *)temp_buf,dev->rx_buf.size - CRC_LEN_OCTET ,(uint8_t *)crc,CRC_LEN_OCTET);
//	uint16_t calc_crc;
//	calc_crc = ntohs(((uint16_t  *) &crc));
//	calc_crc = htons(calc_crc);
//	memcpy(&temp_buf[dev->header_num + PAYLOAD_LEN_OCTET + buf_len],&calc_crc,CRC_LEN_OCTET);
//
////	ec20_write((uint8_t *)temp_buf,dev->header_num + PAYLOAD_LEN_OCTET + buf_len+ CRC_LEN_OCTET);
//	dev->send_frame_count++;
//	DEBUG("[uart_frame]: SEND\r\n");
//	frame_paser_dump_hex((uint8_t*)temp_buf,dev->header_num + PAYLOAD_LEN_OCTET + buf_len + CRC_LEN_OCTET);
//
//	return 0;
//}

uint32_t frame_paser_get_parsed_frame_count(frame_paser_dev_t *dev)
{
	return dev->parsed_frame_count;
}

uint32_t frame_paser_get_error_frame_count(frame_paser_dev_t *dev)
{
	return dev->error_framer_count;
}


