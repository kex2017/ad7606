#include "comm_packet.h"
#include "thread.h"
#include "msg.h"
#include "comm.h"
#include "log.h"

#include <stdio.h>
#include <string.h>


#include "cfg.h"
#include "x_queue.h"
#include "x_delay.h"
#include "frame_common.h"
#include "sc_error_statistics.h"
#include "cable_ground_threads.h"

//*************************SENDER START**********************************
kernel_pid_t comm_packet_sender_pid = KERNEL_PID_UNDEF;
char comm_packet_sender_stack[THREAD_STACKSIZE_MAIN*8] __attribute__((section(".big_data")));


#define PACKET_SENDER_MSG_QUEUE_SIZE (16)
static msg_t packet_sender_msg_queue[PACKET_SENDER_MSG_QUEUE_SIZE];



void *comm_packet_sender(void *arg)
{
	(void)arg;
	msg_t msg;
	PACKET *pkt;

	msg_init_queue(packet_sender_msg_queue, PACKET_SENDER_MSG_QUEUE_SIZE);
	while (1) {
		msg_receive(&msg);
		pkt = (PACKET*)(msg.content.ptr);
		comm_send_data((uint8_t *)pkt->data,pkt->data_len);
	   LOG_DUMP((const char*)pkt->data, pkt->data_len);
		delay_ms(pkt->data_len / 3);
		statistics_snd_num();
	}
	return NULL;
}

kernel_pid_t comm_packet_sender_init(void)
{
	if (comm_packet_sender_pid == KERNEL_PID_UNDEF) {
		comm_packet_sender_pid = thread_create(comm_packet_sender_stack,
				sizeof(comm_packet_sender_stack),
				COMM_PACKET_SENDER_PRIORITY, THREAD_CREATE_STACKTEST,
				comm_packet_sender, NULL, "comm_packet_sender");
	}
	return comm_packet_sender_pid;
}
//*************************SENDER END**********************************


//*************************RECEIVER START******************************
kernel_pid_t comm_packet_receiver_pid = KERNEL_PID_UNDEF;
char comm_packet_receiver_stack[THREAD_STACKSIZE_MAIN*8] __attribute__((section(".big_data")));

kernel_pid_t receiver_pid = KERNEL_PID_UNDEF;

static circular_queue_t DataQueue;

void remove_first_n_item_from_queue(circular_queue_t *data_queue, int32_t queue_len, int32_t n)
{
   int32_t index = 0;
   unsigned char tmp;

   if (n > queue_len) {
      LOG_ERROR("Request item number which try to remove is invalid: request(%d) > current(%d).", n, queue_len);
   }
   for (index = 0; index < n; index++) {
      pop_queue(data_queue, &tmp);
   }
}

boolean find_frame_header(circular_queue_t *data_queue, int32_t queue_len)
{
   int32_t index = 0;

   for (index = 0; index < queue_len; index++) {
      if (FRAME_STARTER == get_queue_item_n(data_queue, index)) {
         if (0 != index) {
            remove_first_n_item_from_queue(data_queue, queue_len, index);
         }
         return SD_TRUE;
      }
   }

   return SD_FALSE;
}

int32_t get_frame_data_field_length(circular_queue_t *data_queue)
{
   return get_queue_item_n(data_queue, FRAME_DATA_LEN_INDEX) << 8 | get_queue_item_n(data_queue, FRAME_DATA_LEN_INDEX + 1);
}

int32_t get_frame_data_length(circular_queue_t *data_queue)
{
   return FRAME_HEADER_LEN + get_frame_data_field_length(data_queue) + FRAME_CS_LEN;
}

boolean is_frame_checksum_mismatch(uint8_t *data, uint32_t length)
{
   uint8_t calc_checksum = 0;
   uint8_t recv_checksum = 0;

   calc_checksum = byte_sum_checksum(data, length - FRAME_CS_LEN);
   recv_checksum = *(data + length - FRAME_CS_LEN);
   if (calc_checksum != recv_checksum) {
      LOG_ERROR("Received invalid frame with recv_checksum(%02X) != calc_checksum(%02X).", recv_checksum,
                calc_checksum);
      statistics_rcv_cs_err_num();
      return SD_FALSE;
   }

   return SD_TRUE;
}

boolean is_slave_addr_mismatch(uint8_t *data)
{
   uint16_t slave_addr = 0;

   slave_addr = data[1] << 8 | data[2];

    if (0 == slave_addr || slave_addr == cfg_get_device_id()) {
        return SD_TRUE;
    } else {
        LOG_DEBUG("Slave addr mismatch: received(%04XH) != actual(%04XH).", slave_addr, cfg_get_device_id());
        return SD_FALSE;
    }
}

boolean start_data_parse(circular_queue_t *queue_data, uint8_t *frame_data, uint32_t *frame_data_len)
{
    int32_t queue_len = 0;
    int32_t package_len = 0;
    uint8_t data[1024] = { 0 };

    queue_len = get_current_queue_data_len(queue_data);
    if ((queue_len < 7) || (SD_TRUE != find_frame_header(queue_data, queue_len))) {
        return SD_FALSE;
    }
    package_len = get_frame_data_length(queue_data);
    if (package_len > 1024) {
        remove_first_n_item_from_queue(queue_data, queue_len, 1);
        return SD_FALSE;
    }
    if (queue_len < package_len) return SD_FALSE;
    if (package_len != read_queue2array_timeout(queue_data, data, package_len, 1)) return SD_FALSE;
    if ((SD_FALSE == is_frame_checksum_mismatch(data, package_len)) || (SD_FALSE == is_slave_addr_mismatch(data))) {
        return SD_FALSE;
    }
    else {
            *frame_data_len = package_len;
            memcpy(frame_data, data, package_len);
    }
    return SD_TRUE;
}

void *comm_packet_receiver(void *arg)
{
	(void) arg;
	PACKET packet;
	msg_t msg;

	while (1) {
		delay_ms(10);
		if (0 < comm_get_data(&DataQueue)) {
			if (SD_TRUE == start_data_parse(&DataQueue, packet.data, &(packet.data_len))) {
				msg.content.ptr = (void *)&packet;
				msg_send(&msg, receiver_pid);
				statistics_rcv_num();
			}
		}
	}
	return NULL;
}

void comm_packet_receiver_hook(kernel_pid_t pid )
{
    receiver_pid = pid;
}

kernel_pid_t comm_packet_receiver_init(void)
{
	if (comm_packet_receiver_pid == KERNEL_PID_UNDEF) {
		comm_packet_receiver_pid = thread_create(comm_packet_receiver_stack,
				sizeof(comm_packet_receiver_stack),
				COMM_PACKET_RECEIVE_PRIORITY, THREAD_CREATE_STACKTEST,
				comm_packet_receiver, NULL, "comm_packet_receiver");
	}
	return comm_packet_receiver_pid;
}
//*************************RECEIVER END******************************
