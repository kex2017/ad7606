#ifndef _DATA_PROCESSOR_H_
#define _DATA_PROCESSOR_H_

#include "thread.h"

uint32_t get_interval_second(void);

void data_processor_thread_init(void);

void add_data_recv_hook(kernel_pid_t pid);

uint8_t *get_data_send_flag(void);

#endif
