#ifndef FRAME_DECODE_H_
#define FRAME_DECODE_H_
#include "frame_common.h"

//uint16_t _frame_decode(uint8_t decode_type, uint8_t* in, frame_header_t* frame_header, uint8_t* out);


uint16_t frame_decode(uint8_t *frame_data, frame_req_t *master_frame_req);

#endif /* FRAME_DECODE_H_ */
