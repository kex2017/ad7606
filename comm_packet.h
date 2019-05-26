#ifndef _COMM_PACKET_H_
#define _COMM_PACKET_H_

#include <stdint.h>
#include "kernel_types.h"
#include <stddef.h>

#define MAX_PACKET_LEN 1500
typedef struct _packet_t{
	uint8_t data[MAX_PACKET_LEN];
	uint32_t data_len;
}PACKET;


kernel_pid_t comm_packet_receiver_init(void);
void comm_packet_receiver_hook(kernel_pid_t subscriber_pid );
kernel_pid_t comm_packet_sender_init(void);



#endif
