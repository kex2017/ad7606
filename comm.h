#ifndef __COMM_H__
#define __COMM_H__

#include <stdint.h>
#include "x_queue.h"

void comm_init(void);
void comm_send_uplink_request(uint8_t *outbuf, uint32_t len);
int comm_get_data(circular_queue_t* data_queue);
void comm_send_data(uint8_t *data, uint16_t len);

#endif
