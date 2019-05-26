#include "frame_encode.h"
#include "cfg.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "temperature.h"
#include "curve_harmonics.h"
#include "daq.h"

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

uint16_t frame_func_code_encode(uint8_t *data, uint8_t func_code)
{
    return frame_uint8_encode(data,func_code);
}

uint16_t frame_abnormal_encode(uint8_t *data, uint8_t errcode)
{
    return frame_uint8_encode(data,errcode);
}

uint16_t frame_version_encode(uint8_t *data, uint8_t *version, uint16_t data_len)
{
	 uint16_t i = 0;

	for (i = 0; i < data_len; i++) {
		*data++ = *version++;
	}

	return data_len;
}

uint16_t frame_get_temperature_encode(uint8_t *data, float *temperature,uint16_t data_len)
{
   uint16_t i = 0, index = 0;
   for(i = 0; i<data_len; i++){
      index += frame_float_encode(data + index, temperature[i]);
   }
   return index;
}


uint16_t frame_channel_encode(uint8_t *data, uint8_t channel)
{
    return frame_uint8_encode(data,channel);
}

uint16_t frame_timestamp_encode(uint8_t *data, uint32_t timestamp)
{
    return frame_uint32_encode(data,timestamp);
}

uint16_t frame_cs_encode(uint8_t *data, uint8_t cs)
{
    return frame_uint8_encode(data,cs);
}

uint16_t frame_sequence_number_encode(uint8_t *data, uint16_t sn)
{
    return frame_uint16_encode(data, sn);
}

uint16_t frame_max_value_encode(uint8_t *data, float max)
{
    return frame_float_encode(data, max);
}

uint16_t frame_file_type_encode(uint8_t *data, uint8_t file_type)
{
    return frame_uint8_encode(data, file_type);
}

uint16_t frame_cur_packet_encode(uint8_t *data, uint16_t cur_packet)
{
    return frame_uint16_encode(data, cur_packet);
}

uint16_t frame_total_packet_encode(uint8_t *data, uint16_t total_packet)
{
    return frame_uint16_encode(data, total_packet);
}

uint16_t frame_packet_data_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len)
{
    uint16_t i = 0;

    for (i = 0; i < packet_len; i++) {
        *data++ = *packet_data++;
    }

    return packet_len;
}

uint16_t frame_curve_data_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len)
{
    uint16_t index = 0;
    uint16_t *p = (uint16_t *)packet_data;

    for (uint16_t i = 0; i < packet_len / 2; i++) {
        index += frame_uint16_encode(data + i * sizeof(short), *p++);
    }

    return index;
}

uint16_t frame_sample_rsp_encode(uint8_t *data, uint8_t errcode, uint8_t channel_count, uint32_t timestamp)
{
    uint16_t index = 0;

    index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_READ_SAMPLE_DATA_RSP_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_READ_SAMPLE_DATA_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_uint8_encode(data + index, channel_count);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_hf_current_jf_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp)
{
    uint16_t index = 0;

    index += frame_header_encode(data, cfg_get_device_id(), packet_len + FRAME_COMMON_EXTRA_LEN);
    index += frame_func_code_encode(data + index, FRAME_HF_CURRENT_JF_CURVE_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_total_packet_encode(data + index, total_packet);
    index += frame_cur_packet_encode(data + index, cur_packet);
    index += frame_curve_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_dielectric_loss_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp)
{
    uint16_t index = 0;


    index += frame_header_encode(data, cfg_get_device_id(), packet_len + FRAME_COMMON_EXTRA_LEN);
    index += frame_func_code_encode(data + index, FRAME_DIELECTRIC_LOSS_CURVE_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_total_packet_encode(data + index, total_packet);
    index += frame_cur_packet_encode(data + index, cur_packet);
    index += frame_curve_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_pf_current_encode(uint8_t *data, float *value)
{
    uint16_t index = 0;

    for (uint8_t i = 0; i < 4; i++) {
        index += frame_float_encode(data + i * sizeof(float), value[i]);
    }

    return index;
}

uint16_t frame_phase_harmonics_encode(uint8_t *data, float *value)
{
   uint16_t index = 0;
   for(uint16_t i = 0; i<60; i++)
   {
      index += frame_float_encode(data+i*sizeof(float), value[i]);
   }
   return index;
}

uint16_t frame_pf_current_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode, uint8_t channel, uint32_t timestamp)
{
    uint16_t index = 0;

    index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_POWER_FREQUENCY_CURRENT_RSP_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_POWER_FREQUENCY_CURRENT_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_packet_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_phase_hamonics_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode, uint8_t channel, uint32_t timestamp)
{
    (void)channel;
    uint16_t index = 0;

    index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_GET_CURRENT_PHASE_HARMONICS_RSP_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_GET_CURRENT_PHASE_HARMONICS_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_packet_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_collector_info_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode, uint8_t channel, uint32_t timestamp)
{
    uint16_t index = 0;

    index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_COLLECTOR_INFO_RSP_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_COLLECTOR_INFO_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_packet_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_over_voltage_event_info_encode(uint8_t *data, uint8_t channel, uint32_t timestamp, uint16_t sn, float max_value)
{
    uint16_t index = 0;

    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_sequence_number_encode(data + index, sn);
    index += frame_max_value_encode(data + index, max_value);

    return index;

}

uint16_t frame_over_voltage_search_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode)
{
    uint16_t index = 0;

    index += frame_header_encode(data + index, cfg_get_device_id(), packet_len + 0x02);
    index += frame_func_code_encode(data + index, FRAME_VOLTAGE_WARNING_UPLOAD_REQ);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_packet_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_over_voltage_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp, uint16_t sn)
{
    uint16_t index = 0;

    index += frame_header_encode(data, cfg_get_device_id(), packet_len + FRAME_OVER_VOLTAGE_EXTRA_LEN);
    index += frame_func_code_encode(data + index, FRAME_VOLTAGE_WARNING_CURVE_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_total_packet_encode(data + index, total_packet);
    index += frame_cur_packet_encode(data + index, cur_packet);
    index += frame_sequence_number_encode(data + index, sn);
    index += frame_curve_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_transfer_file_rsp_encode(uint8_t *data, uint8_t errcode, uint8_t file_type, uint16_t cur_packet)
{
    uint16_t index = 0;

    index += frame_header_encode(data, cfg_get_device_id(), FRAME_TRANSFER_FILE_RSP_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_TRANSFER_FILE_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_file_type_encode(data + index, file_type);
    index += frame_cur_packet_encode(data + index, cur_packet);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}


uint16_t frame_heart_beat_encode(uint8_t *data, uint8_t errcode, uint32_t timestamp)
{
    uint16_t index = 0;
    index += frame_header_encode(data, cfg_get_device_id(), FRAME_HEART_BEAT_EVENT_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_HEART_BEAT_EVENT);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_pd_threshold_rsp_encode(uint8_t *data, uint8_t errcode)
{
    uint16_t index = 0;
    index += frame_header_encode(data, cfg_get_device_id(), FRAME_PD_THRESHOLD_RSP_DATA_LEN);
    index += frame_func_code_encode(data + index, FRAME_PD_THRESHOLD_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_search_pd_rsp_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint8_t errcode)
{
    uint16_t index = 0;
    index += frame_header_encode(data, cfg_get_device_id(), packet_len + 0x02);
    index += frame_func_code_encode(data + index, FRAME_SEARCH_PD_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_packet_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_partial_discharge_curve_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t timestamp)
{
    uint16_t index = 0;

    index += frame_header_encode(data, cfg_get_device_id(), packet_len + FRAME_PARTIAL_DISCHARGE_EXTRA_LEN);
    index += frame_func_code_encode(data + index, FRAME_PD_CURVE_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, timestamp);
    index += frame_total_packet_encode(data + index, total_packet);
    index += frame_cur_packet_encode(data + index, cur_packet);
    index += frame_curve_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_prpd_data_encode(uint8_t *data, uint8_t *packet_data, uint16_t packet_len, uint16_t cur_packet, uint16_t total_packet, uint8_t errcode, uint8_t channel, uint32_t start_timestamp, uint32_t end_timestamp)
{
    uint16_t index = 0;

    index += frame_header_encode(data, cfg_get_device_id(), packet_len + FRAME_PRPD_EXTRA_LEN);
    index += frame_func_code_encode(data + index, FRAME_PRPD_RSP);
    index += frame_abnormal_encode(data + index, errcode);
    index += frame_channel_encode(data + index, channel);
    index += frame_timestamp_encode(data + index, start_timestamp);
    index += frame_timestamp_encode(data + index, end_timestamp);
    index += frame_total_packet_encode(data + index, total_packet);
    index += frame_cur_packet_encode(data + index, cur_packet);
    index += frame_curve_data_encode(data + index, packet_data, packet_len);
    index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

    return index;
}

uint16_t frame_set_current_time_rsp_encode(uint8_t *data, uint8_t errcode)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_SET_DEVIEC_CURRENT_TIME_RSP_DATA_LEN );
	index += frame_func_code_encode(data + index, FRAME_SET_DEVICE_TIME_RSP);
	index += frame_abnormal_encode(data + index, errcode);
	index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

	return index;
}



uint16_t frame_reboot_rsp_encode(uint8_t *data, uint8_t errcode)
{
   uint16_t index = 0;
   uint8_t reboot_ok = 0x01;

   index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_REBOOT_RSP_DATA_LEN);
   index += frame_func_code_encode(data + index, FRAME_REBOOT_FPGA_OR_ARM_RSP);
   index += frame_abnormal_encode(data + index, errcode);
   index += frame_uint8_encode(data+index,reboot_ok);
   index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

   return index;
}

uint16_t frame_send_version_rsp_encode(uint8_t *data,uint8_t* version,uint16_t len ,uint8_t errcode)
{
	uint16_t index = 0;

	index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_GET_DEVICE_VERSION_RSP_DATA_LEN + len);
	index += frame_func_code_encode(data + index, FRAME_GET_DEVICE_VERSION_RSP);
	index += frame_abnormal_encode(data + index, errcode);
	index += frame_version_encode(data + index, version, len);
	index += frame_cs_encode(data + index, byte_sum_checksum(data, index));
	return index;
}

uint16_t frame_send_temperature_rsp_encode(uint8_t *data, float *temperature, uint32_t timestamp, uint8_t errcode)
{
   uint16_t index = 0;
   uint16_t len = 3;

   sc_get_device_temperature();//for DEBUG

   index += frame_header_encode(data + index, cfg_get_device_id(), FRAME_GET_TEMPERATURE_RSP_DATA_LEN);
   index += frame_func_code_encode(data + index, FRAME_GET_TEMPERATURE_RSP);
   index += frame_abnormal_encode(data + index, errcode);
   index += frame_timestamp_encode(data + index, timestamp);
   index += frame_get_temperature_encode(data+index, temperature, len);
   index += frame_cs_encode(data + index, byte_sum_checksum(data, index));

   return index;
}






