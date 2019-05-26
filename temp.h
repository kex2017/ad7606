#ifndef CABLE_GROUND_TEMP_H_
#define CABLE_GROUND_TEMP_H_

#include <stdint.h>
#include "x_queue.h"

int temp_init(void);
int temp_read(circular_queue_t* data_queue);
void temp_send(const uint8_t *data, uint16_t len);
uint16_t temp_crc16_check(uint8_t *puchMsgg, uint8_t usDataLen);

#endif
