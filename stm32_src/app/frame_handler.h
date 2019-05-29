#ifndef FRAME_HANDLER_H_
#define FRAME_HANDLER_H_

#include "frame_common.h"
#include "kernel_types.h"

void frame_handler(frame_req_t *frame_req);
void do_send_dev_info_msg(void);
void msg_send_pack(uint8_t *data, uint32_t data_len);
void request_data_hook(kernel_pid_t pid);
#endif
