#include "frame_encode.h"
#include "frame_common.h"
#include "env_cfg.h"

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

uint16_t frame_cs_encode(uint8_t *data, uint8_t cs)
{
    return frame_uint8_encode(data,cs);
}

uint16_t frame_time_ctrl_data_encode(uint8_t *data, uint8_t errorcode, uint32_t timestamp)
{
	uint16_t index = 0;

	index += frame_header_encode(data+index, cfg_get_device_id(), TIME_CTRL_RSP_DATA_LEN);
	index += frame_uint8_encode(data+index, TIME_CTRL_RSP);
	index += frame_uint8_encode(data+index, errorcode);
	index += frame_uint32_encode(data+index, timestamp);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}

uint16_t frame_get_running_state_encode(uint8_t *data, uint8_t errorcode, uint32_t temp, uint32_t humidity, uint32_t voltage, uint8_t work , uint32_t timestamp )
{
	uint16_t index = 0;

	index += frame_header_encode(data+index, cfg_get_device_id(), GET_RUNNING_STATE_RSP_DATA_LEN);
	index += frame_uint8_encode(data+index, GET_RUNNING_STATE_RSP);
	index += frame_uint8_encode(data+index, errorcode);
	index += frame_uint32_encode(data+index, temp);
	index += frame_uint32_encode(data+index, humidity);
	index += frame_uint32_encode(data+index, voltage);
	index += frame_uint8_encode(data+index, work);
	index += frame_uint32_encode(data+index, timestamp);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}

uint16_t current_cycle_data_encode(uint8_t *data, uint8_t errorcode, float ch1_current, float ch2_current, uint32_t timestamp)
{
	uint16_t index = 0;
	index += frame_header_encode(data + index, cfg_get_device_id(), POWER_CURRENT_RSP_DATA_LEN);
	index += frame_uint8_encode(data + index, POWER_CURRENT_RSP);
	index += frame_uint8_encode(data + index, errorcode);
	index += frame_uint32_encode(data + index, timestamp);
	index += frame_float_encode(data + index, ch1_current);
	index += frame_float_encode(data + index, ch2_current);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t high_current_cycle_data_encode(uint8_t *data, uint8_t errorcode, float ch1_current, float ch2_current, uint32_t timestamp)
{
	uint16_t index = 0;
	index += frame_header_encode(data + index, cfg_get_device_id(), POWER_CURRENT_RSP_DATA_LEN);
	index += frame_uint8_encode(data + index, HIGH_POWER_CURRENT_RSP);
	index += frame_uint8_encode(data + index, errorcode);
	index += frame_uint32_encode(data + index, timestamp);
	index += frame_float_encode(data + index, ch1_current);
	index += frame_float_encode(data + index, ch2_current);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t current_mutation_data_encode(uint8_t * data,  uint8_t errorcode, uint32_t timestamp, uint8_t channel, uint16_t pkg_sum, uint16_t pkg_index, uint8_t * cur_data, uint16_t len )
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, cfg_get_device_id(), CURRENT_WAVE_FORM_RSP_DATA_LEN + len);
	index += frame_uint8_encode(data + index, CURRENT_WAVE_FORM_RSP);
	index += frame_uint8_encode(data + index, errorcode);
	index += frame_uint32_encode(data + index, timestamp);
	index += frame_uint8_encode(data + index, channel);
	index += frame_uint16_encode(data + index, pkg_sum);
	index += frame_uint16_encode(data + index, pkg_index);
	for(int i = 0; i<len; i++ )
	{
		index += frame_uint8_encode(data + index, cur_data[i]);
	}
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}

uint16_t high_current_mutation_data_encode(uint8_t * data,  uint8_t errorcode, uint32_t timestamp, uint8_t channel, uint16_t pkg_sum, uint16_t pkg_index, uint8_t * cur_data, uint16_t len )
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, cfg_get_device_id(), HIGH_CURRENT_WAVE_FORM_RSP_DATA_LEN + len);
	index += frame_uint8_encode(data + index, HIGH_CURRENT_WAVE_FORM_RSP);
	index += frame_uint8_encode(data + index, errorcode);
	index += frame_uint32_encode(data + index, timestamp);
	index += frame_uint8_encode(data + index, channel);
	index += frame_uint16_encode(data + index, pkg_sum);
	index += frame_uint16_encode(data + index, pkg_index);
	for(int i = 0; i<len; i++ )
	{
		index += frame_uint8_encode(data + index, cur_data[i]);
	}
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}

uint16_t dev_running_state_encode(uint8_t * data, uint8_t errorcode, float temp, float humidity, float voltage, uint8_t work, uint32_t timestamp)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, cfg_get_device_id(), GET_RUNNING_STATE_RSP_DATA_LEN);
	index += frame_uint8_encode(data + index, GET_RUNNING_STATE_RSP);
	index += frame_uint8_encode(data + index, errorcode);
	index += frame_float_encode(data + index, temp);
	index += frame_float_encode(data + index, humidity);
	index += frame_float_encode(data + index, voltage);
	index += frame_uint8_encode(data + index, work);
	index += frame_uint32_encode(data + index, timestamp);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}


uint16_t dev_info_encode(uint8_t *data, uint8_t errorcode, uint8_t * version, uint16_t version_len, double longitude, double latitude, double altitude)
{
	uint16_t index= 0;

	index += frame_header_encode(data + index, cfg_get_device_id(), GET_DEV_INFO_RSP_DATA_LEN + version_len);
	index += frame_uint8_encode(data + index, GET_DEV_INFO_RSP);
	index += frame_uint8_encode(data + index, errorcode);
	for(int i = 0; i<version_len; i++ )
	{
		index += frame_uint8_encode(data + index, version[i]);
	}
	index += frame_double_encode(data + index, longitude);
	index += frame_double_encode(data + index, latitude);
	index += frame_double_encode(data + index, altitude);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}

