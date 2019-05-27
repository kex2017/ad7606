#ifndef FRAME_HANDLER_H_
#define FRAME_HANDLER_H_

#include "frame_common.h"
#include "kernel_types.h"

void frame_handler(frame_req_t *frame_req);
void frame_handler_recv_hook(kernel_pid_t pid);
void history_data_send_pid_hook(kernel_pid_t pid);
void comm_handler_sender_hook(kernel_pid_t pid);
void msg_handler_packet_send(uint8_t *data, uint32_t data_len);

void ctrl_sample_info_handler(frame_req_t *frame_req);
#endif
