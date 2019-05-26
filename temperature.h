#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include "kernel_types.h"

#define DEVICE_OFF_LINE_CODE (0x0fff-20)

void *temp_read_handler(void *arg);
kernel_pid_t temp_read_service_init(void);
uint8_t get_temperature(float *abc_temp_buf, uint32_t *timestamp) ;
void sc_get_device_temperature(void);
#endif//TEMPERATURE_H_
