#ifndef _DATA_PROCESSOR_H_
#define _DATA_PROCESSOR_H_

#include "thread.h"

//HfLoadCur
#define HF_CHAN_NUM 2
#define DEFAULT_THRESHOLD 2047
#define DEFAULT_CHANGERATE 4095


uint32_t get_interval_second(void);

void data_processor_thread_init(void);

void add_data_recv_hook(kernel_pid_t pid);

uint8_t *get_data_send_flag(void);

#endif
