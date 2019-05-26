#ifndef FRAME_DECODE_H_
#define FRAME_DECODE_H_

#include <stdint.h>
#include "frame_common.h"

uint16_t frame_decode(uint8_t *frame_data, frame_req_t *master_frame_req);


#endif
