#include "frame_decode.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "log.h"


uint16_t frame_uint16_decode(uint8_t *data)
{
    return data[0] << 8 | data[1];
}

uint32_t frame_uint32_decode(uint8_t* data)
{
   uint32_t i = 0;
   uint32_t data_temp = 0;

   for (i = 0; i < 4; i++) {
      data_temp = (data_temp << 8) | *(data + i);
   }

   return data_temp;
}

uint8_t frame_reboot_decode(uint8_t* data)
{
   uint8_t reboot_type;
   reboot_type = *data;
   return reboot_type;;
}

uint16_t frame_header_decode(uint8_t* data, header_t* header)
{
   header->starter1 = data[0];
   header->slave_addr = frame_uint16_decode(data + 1);
   header->master_addr = frame_uint16_decode(data + 3);
   header->data_len = frame_uint16_decode(data + 5);

   return FRAME_HEADER_LEN;
}

uint16_t frame_func_code_decode(uint8_t* data, uint8_t* func_code)
{
    *func_code = data[0];

    return 1;
}

uint16_t frame_cs_decode(uint8_t* data, uint8_t* cs)
{
    *cs = data[0];

    return 1;
}

uint16_t frame_read_sample_data_req_decode(uint8_t *data, read_sample_data_req_t *req, uint16_t data_len)
{
    uint8_t i = 0;
    uint16_t index = 0;
    channel_cfg_t cfg;
    uint8_t *temp;

    memset(&cfg, 0x0, sizeof(channel_cfg_t));

    req->channel_count = data[0];
    req->timestamp = frame_uint32_decode(data + 1);

    index = 5;
    temp = data + index;
    for (i = 0; i < req->channel_count; i++) {
        cfg.channel = temp[0];
        cfg.max_fre = temp[1];
        cfg.min_fre = temp[2];
        cfg.gain = temp[3];
        cfg.threshold = frame_uint16_decode(temp + 4);
        req->cfg[i] = cfg;

        temp += 6;
    }

    return data_len;
}

uint16_t frame_read_hf_current_jf_curve_req_decode(uint8_t *data, read_hf_current_jf_curve_req_t *req, uint16_t data_len)
{
    req->channel = data[0];
    req->timestamp = frame_uint32_decode(data + 1);

    return data_len;
}

uint16_t frame_read_dielectric_loss_curve_req_decode(uint8_t *data, read_dielectric_loss_curve_req_t *req, uint16_t data_len)
{

    req->channel = data[0];
    req->timestamp = frame_uint32_decode(data + 1);

    return data_len;
}

uint16_t frame_read_pf_current_data_req_decode(uint8_t *data, read_pf_current_data_req_t *req, uint16_t data_len)
{
    req->channel = data[0];
    req->timestamp = frame_uint32_decode(data + 1);

    return data_len;
}

uint16_t frame_get_current_phase_harmonics_decode(uint8_t *data, read_current_phase_req_t *req,uint16_t data_len)
{
   req->channel = data[0];
   req->timestamp = frame_uint32_decode(data + 1);

   return data_len;
}
uint16_t frame_read_collector_info_req_decode(uint8_t *data, read_collector_info_req_t *req, uint16_t data_len)
{
    req->channel = data[0];

    return data_len;
}

uint16_t frame_read_over_voltage_flag_req_decode(uint8_t *data, read_over_voltage_flag_req_t *req, uint16_t data_len)
{
   req->channel = data[0];
   req->threshold[0] = frame_uint16_decode(data + 1);
   req->threshold[1] = frame_uint16_decode(data + 3);
   req->threshold[2] = frame_uint16_decode(data + 5);
   req->threshold[3] = frame_uint16_decode(data + 7);

   return data_len;
}

uint16_t frame_voltage_warning_upload_cnf_decode(uint8_t *data, over_voltage_warning_upload_cnf_t *req, uint16_t data_len)
{
    uint16_t i = 0;
    uint8_t *p = data;

    req->count = (data_len - 1) / 7;
    for (i = 0; i < req->count; i++) {
        req->channel[i] = p[0];
        req->timestamp[i] = frame_uint32_decode(p + 1);
        req->sn[i] = frame_uint16_decode(p + 5);
        p += 7;
    }

    return data_len;
}

uint16_t frame_read_voltage_warning_curve_req_decode(uint8_t *data, read_voltage_warning_curve_req_t *req, uint16_t data_len)
{
    req->channel = data[0];
    req->timestamp = frame_uint32_decode(data + 1);
    req->sn = frame_uint16_decode(data + 5);

    return data_len;
}

uint16_t freme_error_retransmission_req_decode(uint8_t *data, frame_error_retransmission_req_t *req, uint16_t data_len)
{
    uint16_t i = 0;

    req->timestamp = frame_uint32_decode(data);
    req->reqtype = data[4];
    req->channel = data[5];
    req->errcnt = frame_uint16_decode(data + 6);
    req->sn = frame_uint16_decode(data + 8);

    for(i = 0; i < req->errcnt; i++)
    {
        req->errnum[i] = data[i + 10];
    }

    return data_len;
}

uint16_t frame_upload_file_decode(uint8_t* data, transfer_file_req_t* req, uint16_t data_len)
{
    uint8_t file_type = 0;
    uint16_t file_count = 0;
    uint16_t file_index = 0;

    file_type = data[0];
    file_count = frame_uint16_decode(data + 1);
    file_index = frame_uint16_decode(data + 3);
    memcpy(req->md5, data+5, 32);

    req->file_type = file_type;
    req->file_count = file_count;
    req->file_index = file_index;
    req->data_len = data_len - 38;

    memcpy(req->buff, data + 37, req->data_len);

    return data_len;
}

uint16_t frame_pd_threshold_decode(uint8_t* data, frame_pd_threshold_req_t* req, uint16_t data_len)
{
    uint8_t index = 0;

    req->channel_count = (data_len - 1) / 5;
    if (req->channel_count > 3) {
        req->channel_count = 3;
    }

    for(uint8_t i = 0; i < req->channel_count; i++)
    {
        req->channel[i] = data[index];
        req->threshold[i] = frame_uint16_decode(data + 1 + index);
        req->change_rate[i] = frame_uint16_decode(data + 3 + index);
        req->mode[i] = data[5 + index];
        index += 6;
    }

    return data_len;
}

uint16_t frame_search_pd_decode(uint8_t* data, frame_search_pd_req_t* req, uint16_t data_len)
{
    req->start_time = frame_uint32_decode(data);
    req->end_time = frame_uint32_decode(data + 4);

    return data_len;
}

uint16_t frame_pd_curve_decode(uint8_t* data, frame_pd_curve_req_t* req, uint16_t data_len)
{
    req->channel = data[0];
    req->timestamp = frame_uint32_decode(data + 1);
    req->nanosecond = frame_uint32_decode(data + 5);

    return data_len;
}

uint16_t frame_prpd_data_decode(uint8_t* data, frame_prpd_req_t* req, uint16_t data_len)
{
    req->channel = data[0];
    req->timestamp = frame_uint32_decode(data + 1);

    return data_len;
}

uint16_t frame_set_time_decode(uint8_t* data, frame_set_time_t* req, uint16_t data_len)
{
    req->time = frame_uint32_decode(data);
    return data_len;
}

uint16_t frame_get_version_req_decode(uint8_t *data, uint16_t data_len)
{
	(void)data;
	return data_len;
}


uint16_t frame_get_temperature_req_decode(uint8_t *data,frame_get_temperature_req_t* req, uint16_t data_len)
{
   req->timestamp = frame_uint32_decode(data);
   return data_len;
}

uint16_t frame_reboot_req_decode(uint8_t *data,frame_reboot_type_t* req, uint16_t data_len)
{
   req->reboot_type = frame_reboot_decode(data);
   return data_len;
}

uint16_t frame_req_data_decode(uint8_t* frame_data, frame_req_t *master_frame_req)
{
    uint32_t index = 0;
    uint16_t ret = 0;

    index += frame_func_code_decode(frame_data + index, &master_frame_req->func_code);

    switch(master_frame_req->func_code) {
    case FRAME_READ_SAMPLE_DATA_REQ:
        LOG_DEBUG("Get read sample data request from master.");
        ret = frame_read_sample_data_req_decode(frame_data + index, &master_frame_req->frame_req.read_sample_data_req, master_frame_req->header.data_len);
        break;
    case FRAME_HF_CURRENT_JF_CURVE_REQ:
        LOG_DEBUG("Get read high frequency JF curve data request from master.");
        ret = frame_read_hf_current_jf_curve_req_decode(frame_data + index, &master_frame_req->frame_req.read_hf_current_jf_req, master_frame_req->header.data_len);
        break;
    case FRAME_DIELECTRIC_LOSS_CURVE_REQ:
        LOG_DEBUG("Get read dielectric loss curve data request from master.");
        ret = frame_read_dielectric_loss_curve_req_decode(frame_data + index, &master_frame_req->frame_req.read_dielectric_loss_curve_req, master_frame_req->header.data_len);
        break;
    case FRAME_POWER_FREQUENCY_CURRENT_REQ:
        LOG_DEBUG("Get read power frequency current data request from master.");
        ret = frame_read_pf_current_data_req_decode(frame_data + index, &master_frame_req->frame_req.read_pf_current_data_req, master_frame_req->header.data_len);
        break;
    case FRAME_COLLECTOR_INFO_REQ:
        LOG_DEBUG("Get read collector info request from master.");
        ret = frame_read_collector_info_req_decode(frame_data + index, &master_frame_req->frame_req.read_collector_info_req, master_frame_req->header.data_len);
        break;
    case FRAME_OVER_VOLTAGE_FLAG_REQ:
        LOG_DEBUG("Get read over voltage flag request from master.");
        ret = frame_read_over_voltage_flag_req_decode(frame_data + index, &master_frame_req->frame_req.read_over_voltage_flag_req, master_frame_req->header.data_len);
      break;
    case FRAME_VOLTAGE_WARNING_UPLOAD_CNF:
        LOG_DEBUG("Get voltage warning upload confirm from master.");
        ret = frame_voltage_warning_upload_cnf_decode(frame_data + index, &master_frame_req->frame_req.voltage_warning_upload_cnf, master_frame_req->header.data_len);
        break;
    case FRAME_VOLTAGE_WARNING_CURVE_REQ:
        LOG_DEBUG("Get read voltage warning curve data request from master.");
        ret = frame_read_voltage_warning_curve_req_decode(frame_data + index, &master_frame_req->frame_req.read_voltage_warning_curve_req, master_frame_req->header.data_len);
        break;
    case FRAME_ERROR_RETRANSMISSION_REQ:
        LOG_DEBUG("Get error retransmission from master.");
        ret = freme_error_retransmission_req_decode(frame_data + index, &master_frame_req->frame_req.frame_error_retransmission_req, master_frame_req->header.data_len);
        break;
    case FRAME_TRANSFER_FILE_REQ:
        LOG_DEBUG("Get transfer file request from master.");
        ret = frame_upload_file_decode(frame_data + index, &master_frame_req->frame_req.transfer_file_req, master_frame_req->header.data_len);
        break;
    case FRAME_PD_CFG_SET_REQ:
        LOG_DEBUG("Get dp threshold request from master.");
        ret = frame_pd_threshold_decode(frame_data + index, &master_frame_req->frame_req.pd_threshold_req, master_frame_req->header.data_len);
        break;
    case FRAME_SEARCH_PD_REQ:
        LOG_DEBUG("Get search dp request from master.");
        ret = frame_search_pd_decode(frame_data + index, &master_frame_req->frame_req.search_pd_req, master_frame_req->header.data_len);
        break;
    case FRAME_PD_CURVE_REQ:
        LOG_DEBUG("Get pd curve request from master.");
        ret = frame_pd_curve_decode(frame_data + index, &master_frame_req->frame_req.pd_curve_req, master_frame_req->header.data_len);
        break;
    case FRAME_PRPD_REQ:
        LOG_DEBUG("Get prpd data request from master.");
        ret = frame_prpd_data_decode(frame_data + index, &master_frame_req->frame_req.prpd_req, master_frame_req->header.data_len);
        break;
    case FRAME_SET_TIME:
    	LOG_DEBUG("Set device time request from master.");
    	ret = frame_set_time_decode(frame_data + index, &master_frame_req->frame_req.device_time, master_frame_req->header.data_len);
		break;
    case FRAME_GET_DEVICE_VERSION:
		LOG_DEBUG("Get device version request from master.");
		ret = frame_get_version_req_decode(frame_data + index, master_frame_req->header.data_len);
		break;
    case FRAME_GET_TEMPERATURE:
    	LOG_DEBUG("Get temperature request from master.");
    	ret = frame_get_temperature_req_decode(frame_data+index, &master_frame_req->frame_req.get_temperature_req, master_frame_req->header.data_len);
    	break;
    case FRAME_REBOOT_FPGA_OR_ARM:
       LOG_DEBUG("Get reboot request from master.");
       ret = frame_reboot_req_decode(frame_data+index, &master_frame_req->frame_req.reboot_type, master_frame_req->header.data_len);
       break;
    case FRAME_GET_CURRENT_PHASE_HARMONICS:
       LOG_DEBUG("Get current phase harmonics request from master.");
       ret = frame_get_current_phase_harmonics_decode(frame_data + index, &master_frame_req->frame_req.read_current_phase_req,  master_frame_req->header.data_len);
       break;
    default:
        LOG_WARN("Get invalid func code: %02XH, please check again.", master_frame_req->func_code);
        break;
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




