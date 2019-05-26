#include "frame_handler.h"

#include <string.h>
#include <time.h>
#include "comm.h"
#include "log.h"
#include "board.h"
#include "frame_encode.h"
#include "app_upgrade.h"
#include "circular_task.h"
#include "over_voltage.h"
#include "data_acquisition.h"
#include "upgrade_from_flash.h"
#include "periph/rtt.h"
#include "x_delay.h"
#include "cfg.h"
#include "comm_packet.h"
#include "sc_error_statistics.h"
#include "partial_discharge.h"
#include "acquisition_trigger.h"
#include "data_storage.h"
#include "gps_sync.h"
#include "temperature.h"
#include "curve_harmonics.h"
#include "daq.h"

static PACKET handler_packet;

static kernel_pid_t sender_pid;
void comm_handler_sender_hook(kernel_pid_t pid)
{
   sender_pid = pid;
}

void msg_handler_packet_send(uint8_t *data, uint32_t data_len)
{
   msg_t msg;
   handler_packet.data_len = data_len;
   memcpy(handler_packet.data, data, data_len);
   msg.content.ptr = (void *) &handler_packet;
   msg_send(&msg, sender_pid);
}
#define TEN_MIN_COUNT (10 * 60)
static uint32_t g_last_sample_time = 0;
static uint32_t g_this_sample_time = 0;
void set_g_last_sample_time(uint32_t timestamp)
{
    g_last_sample_time = timestamp;
}

uint32_t get_g_last_sample_time(void)
{
    if(g_last_sample_time < 3600)
            g_last_sample_time = rtt_get_counter() - TEN_MIN_COUNT;
    return g_last_sample_time;
}

void set_g_this_sample_time(uint32_t timestamp)
{
    g_this_sample_time = timestamp;
}

uint32_t get_g_this_sample_time(void)
{
    if(g_this_sample_time < 3600)
        g_this_sample_time = rtt_get_counter();
    return g_this_sample_time;
}

void update_sample_time(uint32_t timestamp)
{
    set_g_last_sample_time(get_g_this_sample_time());
    set_g_this_sample_time(timestamp);
}

void read_sample_data_req_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    read_sample_data_req_t *req = &frame_req->frame_req.read_sample_data_req;
    uint8_t errcode = DEVICEOK;
    uint32_t timestamp = rtt_get_counter();

    struct tm *mtime = localtime((time_t*)&(req->timestamp));
    struct tm *ntime = localtime((time_t*)&(timestamp));

    if (req->timestamp > timestamp) {
        update_sample_time(req->timestamp);
        LOG_INFO("Get channel_count:%d with sample time:%d-%02d-%02d %02d:%02d:%02d, now time:%d-%02d-%02d %02d:%02d:%02d request from master",
                 req->channel_count,
                 mtime->tm_year + 1900, mtime->tm_mon + 1, mtime->tm_mday, mtime->tm_hour, mtime->tm_min, mtime->tm_sec,
                 ntime->tm_year + 1900, ntime->tm_mon + 1, ntime->tm_mday, ntime->tm_hour, ntime->tm_min, ntime->tm_sec);
        data_acquisition_set_trigger_sammple_info(req->timestamp, req->channel_count, &req->cfg);
    }
    else {
       errcode = DEVTIMEERR;
       LOG_INFO("Get channel_count:%d with sample time:%d-%02d-%02d %02d:%02d:%02d <= now time:%d-%02d-%02d %02d:%02d:%02d request from master",
                 req->channel_count,
                 mtime->tm_year + 1900, mtime->tm_mon + 1, mtime->tm_mday, mtime->tm_hour, mtime->tm_min, mtime->tm_sec,
                 ntime->tm_year + 1900, ntime->tm_mon + 1, ntime->tm_mday, ntime->tm_hour, ntime->tm_min, ntime->tm_sec);
    }
    length = frame_sample_rsp_encode(data, errcode, req->channel_count, req->timestamp);

    msg_handler_packet_send(data, length);
}

void read_hf_current_jf_curve_req_handler(frame_req_t *frame_req)
{
    uint32_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t total_packet = 0;
    uint16_t list_packet[512] = { 0 };
    read_hf_current_jf_curve_req_t *req = &frame_req->frame_req.read_hf_current_jf_req;

    struct tm *time = localtime((time_t*)&(req->timestamp));
    LOG_INFO("Get channel:%d read hf current jf curve data time :%d-%02d-%02d %02d:%02d:%02d from master", req->channel, time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

    if(0 != data_acquisition_get_curve_data_len(req->channel, req->timestamp, &length))
    {
        length = frame_hf_current_jf_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }

    total_packet = (length / MAX_RSP_DATA_LEN) + !!(length % MAX_RSP_DATA_LEN);

    for(uint16_t i = 0; i < total_packet; i++)
    {
        list_packet[i] = i;
    }

    if(SD_FALSE == set_circular_task_base(req->channel, HF_CURRENT_JF_CURVE, length, total_packet, list_packet, total_packet, req->timestamp, 0))
    {
        length = frame_hf_current_jf_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }

}

void read_dielectric_loss_curve_req_handler(frame_req_t *frame_req)
{
    uint32_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t total_packet = 0;
    uint16_t list_packet[512] = { 0 };

    read_dielectric_loss_curve_req_t *req = &frame_req->frame_req.read_dielectric_loss_curve_req;

    struct tm *time = localtime((time_t*)&(req->timestamp));
    LOG_INFO("Get channel:%d read %d-%02d-%02d %02d:%02d:%02d dielectric loss curve data from master", req->channel, time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

    if(0 != data_acquisition_get_curve_data_len(req->channel, req->timestamp, &length))
    {
        length = frame_dielectric_loss_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }

    total_packet = (length / MAX_RSP_DATA_LEN) + !!(length % MAX_RSP_DATA_LEN);

    for(uint16_t i = 0; i < total_packet; i++)
    {
        list_packet[i] = i;
    }

    if(SD_FALSE == set_circular_task_base(req->channel, DIELECTRIC_LOSS_CURVE, length, total_packet, list_packet, total_packet, req->timestamp, 0))
    {
        length = frame_dielectric_loss_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }
}

void read_pf_current_data_req_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    float abc[4] = { 0 };
    uint8_t packet_data[16] = { 0 };
    uint16_t packet_len = 0;
    struct tm *time = NULL;

    read_pf_current_data_req_t *req = &frame_req->frame_req.read_pf_current_data_req;
    time = localtime((time_t*)&(req->timestamp));
    LOG_INFO("Get read pf_current data time :%d-%02d-%02d %02d:%02d:%02d from master", time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

    if (0 == data_acquisition_read_pf_current(req->channel, req->timestamp, (void*)abc)) {
        packet_len = frame_pf_current_encode(packet_data,abc);
        length = frame_pf_current_rsp_encode(data, packet_data, packet_len, DEVICEOK, req->channel, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }
    length = frame_pf_current_rsp_encode(data, packet_data, 0x10, SAMPLEERR, req->channel, req->timestamp);
    msg_handler_packet_send(data, length);
}

void read_collector_info_req_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    uint8_t info[64] = { "This is a acquisition card equipment." };
    uint16_t packet_len = 0;
    read_collector_info_req_t *req = &frame_req->frame_req.read_collector_info_req;

    LOG_INFO("Get collector info from master");
    packet_len += frame_double_encode(packet_data, cfg_get_device_longitude());
    packet_len += frame_double_encode(packet_data + sizeof(double), cfg_get_device_latitude());
    packet_len += frame_double_encode(packet_data + 2 * sizeof(double), cfg_get_device_height());
    memcpy(packet_data+ 3 * sizeof(double),info,strlen((const char*)info));
    packet_len += sizeof(info);

    length = frame_collector_info_rsp_encode(data, packet_data, packet_len, DEVICEOK, req->channel, rtt_get_counter());
    msg_handler_packet_send(data, length);
}

uint16_t get_over_voltage_event_packet(uint8_t *data, uint8_t *channels, uint8_t count)
{
    uint16_t index = 0;
    float max_value = 0.0;
    over_voltage_event_info_t event;

    if (0 == count) {
        index = frame_over_voltage_event_info_encode(data, -1, 0xffffffff, 0xffff, 0xffffffff);
        return index;
    }

    for (uint8_t i = 0; i < count; i++) {
        over_voltage_get_event_info(channels[i], &event);
        LOG_INFO("Got channel[%d] over voltage details: time:%s, sn:%d, max_value:%d, k:%d", channels[i], ctime((const time_t*)&(event.timestamp)), event.sn, event.max_value, event.k);
        max_value = event.max_value * event.k;
        index += frame_over_voltage_event_info_encode(data + index, channels[i], event.timestamp, event.sn, max_value);
        memset(&event, 0x0, sizeof(over_voltage_event_info_t));
    }

    return index;
}

void read_over_voltage_search_req_handler(frame_req_t *frame_req)
{
    uint16_t length = 0;
    uint8_t data[ MAX_RSP_FRAME_LEN] = { 0 };
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    uint16_t packet_len = 0;
    uint8_t channels[4] = { 0 };
    uint8_t count = 0;

    read_over_voltage_flag_req_t *req = &frame_req->frame_req.read_over_voltage_flag_req;

    LOG_INFO("Get over voltage query from master with threshold: channel(0)=%d, channel(1)=%d, channel(2)=%d, channel(3)=%d.",
    		req->threshold[0], req->threshold[1], req->threshold[2], req->threshold[3]);
    for (uint8_t channel = 0; channel < MAX_OVER_VOLTAGE_CHANNEL_COUNT; channel++) {
        over_voltage_set_threshold(channel, req->threshold[channel]);
    }

    over_voltage_get_event_happened_channels(channels, &count);
    LOG_INFO("Query over voltage event count %d happen", count);

    if (0 != count) {
        packet_len = get_over_voltage_event_packet(packet_data, channels, count);

    }
    else {
        packet_len = get_over_voltage_event_packet(packet_data, NULL, 0);
    }

    length = frame_over_voltage_search_rsp_encode(data, packet_data, packet_len, DEVICEOK);
    msg_handler_packet_send(data, length);
}

void read_over_voltage_warning_curve_req_handler(frame_req_t *frame_req)
{
    uint32_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t total_packet = 0;
    uint16_t list_packet[512] = { 0 };

    read_voltage_warning_curve_req_t *req = &frame_req->frame_req.read_voltage_warning_curve_req;
    clear_channel_over_voltage_happened_flag(req->channel);

    LOG_INFO("Get channel:%d timesamp:%d sn:%d over voltage warning curve data from master",req->channel, req->timestamp, req->sn);
    if (0 != over_voltage_get_curve_data_len(req->channel, req->timestamp, &length)) {
        length = frame_over_voltage_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp, req->sn);
        msg_handler_packet_send(data, length);
        return;
    }

    total_packet = (length / MAX_RSP_DATA_LEN) + !!(length % MAX_RSP_DATA_LEN);

    for (uint16_t i = 0; i < total_packet; i++) {
        list_packet[i] = i;
    }

    if(SD_FALSE == set_circular_task_base(req->channel, VOLTAGE_WARNING_CURVE, length, total_packet, list_packet, total_packet, req->timestamp, req->sn))
    {
        length = frame_over_voltage_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp, req->sn);
        msg_handler_packet_send(data, length);
        return;
    }
}

void error_retransmission_hf_current_rsp(uint8_t channel, uint16_t count, uint8_t *num, uint32_t timestamp)
{
    uint16_t list_packet[512] = { 0 };

    LOG_WARN("Get channel:%d timesamp:%d count:%d error_retransmission hf current curve data from master",channel, timestamp, count);


    for(uint16_t i = 0; i < count; i++)
    {
        list_packet[i] = num[i];
    }

    set_circular_task_base(channel, HF_CURRENT_JF_CURVE_ERR, 1, 1, list_packet, count, timestamp, 0);


}

void error_retransmission_dielectric_loss_rsp(uint8_t channel, uint16_t count, uint8_t *num, uint32_t timestamp)
{
    uint16_t list_packet[512] = { 0 };

    LOG_INFO("Get channel:%d timesamp:%d count:%d error retransmission dielectric loss curve data from master",channel, timestamp, count);

    for(uint16_t i = 0; i < count; i++)
    {
        list_packet[i] = num[i];
    }

    set_circular_task_base(channel, DIELECTRIC_LOSS_CURVE_ERR, 1, 1, list_packet, count, timestamp, 0);
}

void error_retransmission_voltage_warning_rsp(uint8_t channel, uint16_t count, uint8_t *num, uint32_t timestamp, uint16_t sn)
{
    uint16_t list_packet[512] = { 0 };

    LOG_INFO("Get channel:%d timesamp:%d count:%d sn:%d error retransmission voltage warning curve data from master",channel, timestamp, count, sn);

    for(uint16_t i = 0; i < count; i++)
    {
        list_packet[i] = num[i];
    }

    set_circular_task_base(channel, VOLTAGE_WARNING_CURVE_ERR, 1, 1, list_packet, count, timestamp, sn);
}

void error_retransmission_partial_discharge_rsp(uint8_t channel, uint16_t count, uint8_t *num, uint32_t timestamp)
{
    uint16_t list_packet[512] = { 0 };

    LOG_INFO("Get channel:%d timesamp:%d count:%d error retransmission partial discharge curve data from master",channel, timestamp, count);

    for(uint16_t i = 0; i < count; i++)
    {
        list_packet[i] = num[i];
    }

    set_circular_task_base(channel, PARTIAL_DISCHARGE_CURVE_ERR, 1, 1, list_packet, count, timestamp, 0);
}


void error_retransmission_req_handler(frame_req_t *frame_req)
{
    frame_error_retransmission_req_t req = frame_req->frame_req.frame_error_retransmission_req;

    if (req.errcnt <= 0) return;

    switch (req.reqtype) {
    case FRAME_HF_CURRENT_JF_CURVE_REQ:
        error_retransmission_hf_current_rsp(req.channel, req.errcnt, req.errnum, req.timestamp);
        statistics_hf_current_jf_error_num(req.channel, req.errcnt);
        break;
    case FRAME_DIELECTRIC_LOSS_CURVE_REQ:
        error_retransmission_dielectric_loss_rsp(req.channel, req.errcnt, req.errnum, req.timestamp);
        statistics_dielectric_loss_error_num(req.channel, req.errcnt);
        break;
    case FRAME_VOLTAGE_WARNING_CURVE_REQ:
        error_retransmission_voltage_warning_rsp(req.channel, req.errcnt, req.errnum, req.timestamp, req.sn);
        statistics_over_voltage_error_num(req.channel, req.errcnt);
        break;
    case FRAME_PD_CURVE_REQ:
        error_retransmission_partial_discharge_rsp(req.channel, req.errcnt, req.errnum, req.timestamp);
        statistics_partial_discharge_error_num(req.channel, req.errcnt);
        break;
    default:
        return;
    }
}

void upload_file_req_handler(frame_req_t *frame_req)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t file_count = 0;
    uint16_t file_index = 0;
    uint8_t file_type = 0;
    uint8_t  last_packet = 1;
    uint16_t length = 0;
    uint8_t errcode = DEVICEOK;

    file_count = frame_req->frame_req.transfer_file_req.file_count;
    file_index = frame_req->frame_req.transfer_file_req.file_index;
    file_type = frame_req->frame_req.transfer_file_req.file_type;

    if (file_index >= file_count) {
        LOG_WARN("upload file request frame: file index is wrong.file_index(%d), file_count(%d)", file_index, file_count);
        errcode = TRANSFERERR;
    }else if (SD_TRUE != process_upload_file_req(&frame_req->frame_req.transfer_file_req, &last_packet)) {
        LOG_ERROR("process upload file request failed!, file_index(%d), file_count(%d), file_type(%d)", file_index, file_count, file_type);
        errcode = TRANSFERERR;
    }

    LOG_INFO("Upload file pkt info: type:%d, total:%d, index:%d, last_flag:%d", file_type, file_count, file_index, last_packet);
    if (SD_TRUE == last_packet) {
    	errcode = check_upgrade_program_interigy(frame_req->frame_req.transfer_file_req.md5, file_type);
    	if(DEVICEOK == errcode){
    		errcode = process_upgrade_program(file_type);
    	}
    	length = frame_transfer_file_rsp_encode(data, errcode, frame_req->frame_req.transfer_file_req.file_type,file_index);
    	msg_handler_packet_send(data, length);
    }else {
    	length = frame_transfer_file_rsp_encode(data, errcode, frame_req->frame_req.transfer_file_req.file_type,file_index);
    	msg_handler_packet_send(data, length);
    }
}

void pd_cfg_set_handler(frame_req_t *frame_req)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t errcode = DEVICEOK;
    frame_pd_threshold_req_t *req = &frame_req->frame_req.pd_threshold_req;

    for (uint8_t i = 0; i < req->channel_count; i++) {
        if (0 == req->channel[i] || 1 == req->channel[i] || 2 == req->channel[i]) {
            errcode = (uint8_t)partial_discharge_set_channel_cfg_by_index(req->channel[i], req->threshold[i], req->change_rate[i]);
            LOG_INFO("Get channel:%d threshold:%d change_rate:%d set partial discharge threshold data from master",req->channel[i],req->threshold[i], req->change_rate[i]);
        }
    }
    //模式0.自动捕获 ; 模式1.后台控制
    daq_fsmc_pd_work_mode_set(req->mode[0]);
    if (req->mode[0]) {
        LOG_INFO("fpga work at background control mode");
    }
    else {
        LOG_INFO("fpga work at automatically mode");
    }

    length = frame_pd_threshold_rsp_encode(data, errcode);
    msg_handler_packet_send(data, length);
}

void get_phase_harmonics_data(float* data, uint32_t *timestamp)
{
     channel_harmonics_info_t *harmonics_info;

//     read_current_phase_harmonics_data(0);
     uint16_t ha_cunt = 0;

     for(uint8_t i = 0; i < 6 ;i++)
     {
        harmonics_info = get_harmonics_info_by_channel(i+4);
        data[i] = harmonics_info->base_phase;
        for (uint8_t j = 1; j < 10; j++) {
            data[ha_cunt + 6] = harmonics_info->harmonics[j];
            ha_cunt++;
        }
     }
     *timestamp = get_channel_sample_timestamp(4);//4-6通道的采集时间间隔在秒级以内，故可以使用任意通道时间
}

void get_current_phase_harmonics_handler(frame_req_t *frame_req)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint32_t timestamp;
    uint16_t length = 0;
    float abc[4] = { 0 };
    uint16_t packet_len = 0;
    float ph_data[60] = { 0 };
    uint8_t packet_data[262] = { 0 };

    read_pf_current_data_req_t *req = &frame_req->frame_req.read_pf_current_data_req;

    if (0 == data_acquisition_read_pf_current(req->channel, req->timestamp, (void*)abc)) {
        get_phase_harmonics_data(ph_data, &timestamp);
        packet_len = frame_pf_current_encode(packet_data, abc);
        packet_len += frame_phase_harmonics_encode(packet_data + packet_len, ph_data);
        length = frame_phase_hamonics_rsp_encode(data, packet_data, packet_len, DEVICEOK, req->channel, timestamp);
        msg_handler_packet_send(data, length);
        LOG_INFO("Get harmonics info request for timestamp(%ld) from master", req->timestamp);
    }
    else {
        length = frame_phase_hamonics_rsp_encode(data, packet_data, packet_len, SAMPLEERR, req->channel, timestamp);
        msg_handler_packet_send(data, length);
        LOG_WARN("Get harmonics info request for timestamp(%ld) from master, and sample data error!", req->timestamp);
    }


    return;
}

uint8_t get_pd_event_info(frame_search_pd_req_t *req, partial_discharge_event_info_t info[][MAX_PARTIAL_DISCHARGE_EVENT_COUNT], uint8_t *channels, uint8_t *channel_count, uint8_t *sn, uint8_t *sn_count)
{
    uint8_t errcode = DEVICEOK;

    (void)req;
    for(uint8_t channel = 0; channel < MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT; channel++)
    {
        errcode = partial_discharge_query_event_info(channel, info[channel]);
        for(uint8_t i = 0; i < MAX_PARTIAL_DISCHARGE_EVENT_COUNT; i++)
        {
            if((1 == info[channel][i].happened_flag) /*&& (req->start_time <= info[channel][i].second) && (info[channel][i].second <= req->end_time)*/)
            {
                sn[sn_count[channel]] = i;
                sn_count[channel]++;
            }
        }
        if(0 != sn_count[channel])
        {
            channels[*channel_count] = channel;
            (*channel_count)++;
        }
    }

    return errcode;
}

uint16_t get_pd_info_packet_data(uint8_t *data, partial_discharge_event_info_t info[][MAX_PARTIAL_DISCHARGE_EVENT_COUNT], uint8_t *channels, uint8_t channel_count, uint8_t *sn, uint8_t *sn_count)
{
    uint16_t index = 0;

    index += frame_uint8_encode(data, channel_count);
    for(uint8_t i = 0; i < channel_count; i++)
    {
        index += frame_uint8_encode(data + index, channels[i]);
        index += frame_uint8_encode(data + index, sn_count[channels[i]]);
        for(uint8_t j = 0; j < sn_count[channels[i]]; j++)
        {
            index += frame_uint32_encode(data + index, info[channels[i]][sn[j]].second);
            index += frame_uint32_encode(data + index, info[channels[i]][sn[j]].nanosecond);
        }
    }

    return index;
}

void search_pd_req_handler(frame_req_t *frame_req)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    uint16_t packet_len = 0;
    uint8_t errcode = DEVICEOK;
    uint8_t channel_count = 0;
    uint8_t channels[MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT] = { 0 };
    uint8_t sn_count[MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT] = { 0 };
    uint8_t sn[MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT] = { 0 };
    partial_discharge_event_info_t event_info[MAX_PARTIAL_DISCHARGE_CHANNEL_COUNT][MAX_PARTIAL_DISCHARGE_EVENT_COUNT] = { 0 };
    frame_search_pd_req_t *req = &frame_req->frame_req.search_pd_req;

    LOG_INFO("Get start_time:%d end_time:%d search partial discharge event data from master",req->start_time, req->end_time);

    errcode = get_pd_event_info(req, event_info, channels, &channel_count, sn, sn_count);
    if (0 == channel_count) {
        packet_data[0] = channel_count;
        length = frame_search_pd_rsp_encode(data, packet_data, 1, errcode);
        LOG_INFO("No partial discharge event found.");
    }
    else {
        LOG_INFO("Found partial discharge event happened with timestamp:%ld.", event_info[0][0].second);
        packet_len = get_pd_info_packet_data(packet_data, event_info, channels, channel_count, sn, sn_count);
        length = frame_search_pd_rsp_encode(data, packet_data, packet_len, errcode);
    }

    msg_handler_packet_send(data, length);
}

void read_pd_curve_req_handler(frame_req_t *frame_req)
{
    uint16_t sn = 0;
    uint32_t length = 0;
    uint16_t total_packet = 0;
    uint16_t list_packet[512] = { 0 };
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    frame_pd_curve_req_t *req = &frame_req->frame_req.pd_curve_req;

    LOG_INFO("Get channel:%d timesamp:%d nanosecond:%d partial discharge curve data from master",req->channel, req->timestamp, req->nanosecond);

    if(0 > partial_discharge_get_sn(req->channel, req->timestamp, req->nanosecond, &sn))
    {
        goto err;
    }
    if(0 > partial_discharge_get_curve_data_len(req->channel, sn, &length))
    {
        goto err;
    }
    total_packet = (length / MAX_RSP_DATA_LEN) + !!(length % MAX_RSP_DATA_LEN);

    for (uint16_t i = 0; i < total_packet; i++) {
        list_packet[i] = i;
    }

    if(SD_FALSE == set_circular_task_base(req->channel, PARTIAL_DISCHARGE_CURVE, length, total_packet, list_packet, total_packet, req->timestamp, sn))
    {
        goto err;
    }
    return;

    err:
        length = frame_partial_discharge_curve_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp);
        msg_handler_packet_send(data, length);
}

void read_prpd_req_handler(frame_req_t *frame_req)
{
    uint32_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t total_packet = 0;
    uint16_t list_packet[512] = { 0 };
    uint32_t channel = 0;

    frame_prpd_req_t *req = &frame_req->frame_req.prpd_req;
    channel = req->channel + 13;

    LOG_INFO("Get channel:%d timesamp:%d prpd data from master",channel, req->timestamp);
    if ((length = daq_fsmc_sample_len_get(channel)) == 0) {
        length = frame_prpd_data_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }
    length = (length > MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN) ? MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN : length;
    total_packet = (length / MAX_RSP_DATA_LEN) + !!(length % MAX_RSP_DATA_LEN);

    for (uint16_t i = 0; i < total_packet; i++) {
        list_packet[i] = i;
    }

    if(SD_FALSE == set_circular_task_base(req->channel, PRPD_DATA, length, total_packet, list_packet, total_packet, req->timestamp, 0))
    {
        length = frame_prpd_data_encode(data, NULL, 0, 0, 0, SAMPLEERR, req->channel, req->timestamp, req->timestamp);
        msg_handler_packet_send(data, length);
        return;
    }
}

void set_device_time_handler(frame_req_t *frame_req)
{
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	uint16_t length = 0;
	uint8_t errcode = DEVICEOK;
	frame_set_time_t device_time = frame_req->frame_req.device_time;

	LOG_INFO("set timestamp is %u", device_time.time);//TODO:测试是否是以下两句影响了后台设置的时间
//	struct tm *t = localtime((const time_t *) &device_time);
//	LOG_INFO("set time:%d-%02d-%02d %02d:%02d:%02d\n", t->tm_year + 1900, t->tm_mon + 1,t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	rtt_set_counter(device_time.time);
//	init_device_irq();
	length = frame_set_current_time_rsp_encode(data, errcode);
	msg_handler_packet_send(data, length);
}

static char string[128] = {0};
static uint16_t  common_fpga_version[32];//ov_fpga_version[32],
int get_version_info(uint8_t * version)
{
    char fsmc_fpga_version[40] = {0};

    memset(string, 0, sizeof(string));
    memset(common_fpga_version, 0, sizeof(common_fpga_version));

   daq_fsmc_fpga_version_get(common_fpga_version);
   for(int i = 0; i < 32; i++){
       fsmc_fpga_version[i] =  common_fpga_version[i];
   }

   snprintf((char*)string,
          sizeof(string),
         "{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"  FPGA version:\"%s}"
         ,"DN",APPLICATION,"PN",RIOT_BOARD,"HW","00A1","SW", GIT_VERSION, fsmc_fpga_version);

   strncpy((char*)version,(char*)string,strlen(string));

   return strlen(string);
}

void send_device_version_handler(void)
{
	uint32_t length = 0;
	uint16_t len = 0;
	uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
	uint8_t  version[512] = {0};

	len = get_version_info(version);

	length = frame_send_version_rsp_encode(data, version, len, DEVICEOK);
	msg_handler_packet_send(data, length);
	LOG_INFO("send version info success!");
}

void reboot_fpga_or_arm(frame_req_t * frame_req)
{
   uint32_t length = 0;
   uint8_t reboot_type = frame_req->frame_req.reboot_type.reboot_type;
   uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };

   length = frame_reboot_rsp_encode(data, DEVICEOK);
   msg_handler_packet_send(data, length);

   delay_s(1);
   switch (reboot_type) {
   case 0:
      daq_fsmc_fpga_power_reset();
      break;
   case 1:
      soft_reset();
      break;
   }
}


void get_device_temperature_handler(frame_req_t *frame_req)
{
    uint32_t length = 0, timestamp = 0;
    uint8_t errcode;
    float temperature[3];

    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    (void)frame_req;
    errcode = get_temperature(temperature, &timestamp);
    length = frame_send_temperature_rsp_encode(data, temperature, timestamp, errcode);
    msg_handler_packet_send(data, length);
    LOG_INFO("start send device temperature...");
}

void frame_handler(frame_req_t *frame_req)
{
    switch (frame_req->func_code) {
    case FRAME_READ_SAMPLE_DATA_REQ:
        read_sample_data_req_handler(frame_req);
        break;
    case FRAME_HF_CURRENT_JF_CURVE_REQ:
        read_hf_current_jf_curve_req_handler(frame_req);
        break;
    case FRAME_DIELECTRIC_LOSS_CURVE_REQ:
        read_dielectric_loss_curve_req_handler(frame_req);
        break;
    case FRAME_POWER_FREQUENCY_CURRENT_REQ:
        read_pf_current_data_req_handler(frame_req);
        break;
    case FRAME_COLLECTOR_INFO_REQ:
        read_collector_info_req_handler(frame_req);
        break;
    case FRAME_OVER_VOLTAGE_FLAG_REQ:
        read_over_voltage_search_req_handler(frame_req);
        break;
    case FRAME_VOLTAGE_WARNING_CURVE_REQ:
        read_over_voltage_warning_curve_req_handler(frame_req);
        break;
    case FRAME_ERROR_RETRANSMISSION_REQ:
        error_retransmission_req_handler(frame_req);
        break;
    case FRAME_TRANSFER_FILE_REQ:
        upload_file_req_handler(frame_req);
        break;
    case FRAME_PD_CFG_SET_REQ:
        pd_cfg_set_handler(frame_req);
        break;
    case FRAME_SEARCH_PD_REQ:
        search_pd_req_handler(frame_req);
        break;
    case FRAME_PD_CURVE_REQ:
        read_pd_curve_req_handler(frame_req);
        break;
    case FRAME_PRPD_REQ:
        read_prpd_req_handler(frame_req);
        break;
    case FRAME_SET_TIME:
        set_device_time_handler(frame_req);
    	break;
    case FRAME_GET_DEVICE_VERSION:
	    send_device_version_handler();
	    break;
    case FRAME_GET_TEMPERATURE:
        get_device_temperature_handler(frame_req);
        break;
    case FRAME_REBOOT_FPGA_OR_ARM:
        reboot_fpga_or_arm(frame_req);
        break;
    case FRAME_GET_CURRENT_PHASE_HARMONICS:
        get_current_phase_harmonics_handler(frame_req);
        break;
    default:
        LOG_WARN("Get invalid func code: %02XH, please check again.", frame_req->func_code);
        break;
    }
}
