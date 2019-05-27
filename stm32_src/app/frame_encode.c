#include "frame_encode.h"
#include "frame_common.h"

uint16_t frame_double_encode(uint8_t *data,double value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[7];
    data[1] = p[6];
    data[2] = p[5];
    data[3] = p[4];
    data[4] = p[3];
    data[5] = p[2];
    data[6] = p[1];
    data[7] = p[0];

    return sizeof(double);
}

uint16_t frame_float_encode(uint8_t *data,float value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[3];
    data[1] = p[2];
    data[2] = p[1];
    data[3] = p[0];

    return sizeof(float);
}

uint16_t frame_uint32_encode(uint8_t *data,uint32_t value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[3];
    data[1] = p[2];
    data[2] = p[1];
    data[3] = p[0];

    return sizeof(unsigned int);
}

uint16_t frame_uint64_encode(uint8_t *data,uint64_t value)
{
	uint8_t *p = (uint8_t *)&value;

    data[0] = p[7];
    data[1] = p[6];
    data[2] = p[5];
    data[3] = p[4];
    data[4] = p[3];
    data[5] = p[2];
    data[6] = p[1];
    data[7] = p[0];

	return sizeof(uint64_t);
}

uint16_t frame_uint16_encode(uint8_t *data,uint16_t value)
{
    uint8_t *p = (uint8_t *)&value;

    data[0] = p[1];
    data[1] = p[0];

    return sizeof(unsigned short);
}

uint16_t frame_uint8_encode(uint8_t *data,uint8_t value)
{
    data[0] = value;

    return sizeof(unsigned char);
}

uint16_t frame_header_encode(uint8_t *data, uint16_t salve_addr, uint16_t rsp_len)
{
    uint16_t index = 1;

    data[0] = FRAME_STARTER;
    index += frame_uint16_encode(data + index, MASTER_ADDR);
    index += frame_uint16_encode(data + index, salve_addr);
    index += frame_uint16_encode(data + index, rsp_len);

    return FRAME_HEADER_LEN;
}




