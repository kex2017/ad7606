#ifndef _COMM_PACKET_H_
#define _COMM_PACKET_H_

#include <stdint.h>
#include "kernel_types.h"
#include <stddef.h>
#include "type_alias.h"

#define MAX_PACKET_LEN 1024
typedef struct _packet_t{
	uint8_t data[MAX_PACKET_LEN];
	uint32_t data_len;
}PACKET;

typedef struct _break_link_time{
    uint32_t break_time;
    uint32_t linked_time;
    uint8_t dev_link_breaked_flag;
}bl_time_t;

kernel_pid_t comm_packet_receiver_init(void);
void comm_packet_receiver_hook(kernel_pid_t subscriber_pid );
kernel_pid_t get_comm_packet_receiver_pid(void);
kernel_pid_t comm_packet_sender_init(void);

kernel_pid_t history_data_send_init(void);
int get_disconnect_register_flag(void);
void set_history_time_interval(uint32_t start_time , uint32_t end_time, uint8_t type);
void set_disconnect_register_flag(int value);

bl_time_t* get_bl_time(void);
#endif
