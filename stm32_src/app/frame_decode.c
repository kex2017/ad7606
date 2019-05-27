#include "frame_decode.h"

uint16_t frame_uint16_decode(uint8_t *data)
{
    return data[1] << 8 | data[0];
}

uint32_t frame_uint32_decode(uint8_t* data)
{
    return (data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]);
}

uint16_t frame_cs_decode(uint8_t* data, uint8_t* cs)
{
    *cs = data[0];

    return 1;
}

uint16_t frame_func_code_decode(uint8_t* data, uint8_t* func_code)
{
    *func_code = data[0];

    return 1;
}

uint16_t frame_header_decode(uint8_t* data, frame_header_t* header)
{
   header->starter1 = data[0];
   header->slave_addr = frame_uint16_decode(data + 1);
   header->master_addr = frame_uint16_decode(data + 3);
   header->data_len = frame_uint16_decode(data + 5);

   return FRAME_HEADER_LEN;
}

uint16_t frame_req_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    uint32_t index = 0;
    uint16_t ret = 0;

    index += frame_func_code_decode(frame_data + index, &master_frame_req->func_code);
    switch(master_frame_req->func_code) {
    }

    return ret;
}


uint16_t frame_decode(uint8_t *frame_data, frame_req_t *master_frame_req)
{
    uint16_t index = 0;
    index += frame_header_decode(frame_data, &master_frame_req->header);
    index += frame_req_data_decode(frame_data + index, master_frame_req);
    index += frame_cs_decode(frame_data + index, &master_frame_req->cs);

    return index;
}

