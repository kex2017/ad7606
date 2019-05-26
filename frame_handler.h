#ifndef FRAME_HANDLER_H_
#define FRAME_HANDLER_H_

#include "frame_common.h"
#include "kernel_types.h"

void comm_handler_sender_hook(kernel_pid_t pid);
void frame_handler(frame_req_t *frame_req);
uint32_t get_g_this_sample_time(void);
uint32_t get_g_last_sample_time(void);
void get_phase_harmonics_data(float* data, uint32_t *timestamp);
#define FIX_TIME (8 * 60 * 60)
#endif
