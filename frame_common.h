#ifndef FRAME_H_
#define FRAME_H_

#include <stdint.h>
#include "type_alias.h"

#define FRAME_STARTER  0xDB
#define FRAME_HEADER_LEN 0X07
#define FRAME_CS_LEN 0X01

#define FRAME_DATA_LEN_INDEX 0x05

#define MAX_REQ_DATA_LEN 200
#define MAX_REQ_FRAME_LEN (FRAME_HEADER_LEN + MAX_REQ_DATA_LEN + FRAME_CS_LEN)

#define MAX_RSP_DATA_INFO_LEN 32
#define MAX_RSP_DATA_LEN 250
#define BUFFER_LEN  1024
#define MAX_RSP_FRAME_LEN (FRAME_HEADER_LEN + MAX_RSP_DATA_INFO_LEN + MAX_RSP_DATA_LEN + FRAME_CS_LEN)


//frame func code
#define FRAME_READ_SAMPLE_DATA_REQ 0X50
#define FRAME_READ_SAMPLE_DATA_RSP 0X51
#define FRAME_READ_SAMPLE_DATA_RSP_DATA_LEN 0X0007

#define FRAME_HF_CURRENT_JF_CURVE_REQ 0X52
#define FRAME_HF_CURRENT_JF_CURVE_RSP 0X53
#define FRAME_HF_CURRENT_JF_CURVE_CNF 0X54

#define FRAME_COMMON_EXTRA_LEN 0X0B
#define FRAME_OVER_VOLTAGE_EXTRA_LEN 0X0D
#define FRAME_PARTIAL_DISCHARGE_EXTRA_LEN 0X0B
#define FRAME_PRPD_EXTRA_LEN 0X0F

#define FRAME_DIELECTRIC_LOSS_CURVE_REQ 0X56
#define FRAME_DIELECTRIC_LOSS_CURVE_RSP 0X57
#define FRAME_DIELECTRIC_LOSS_CURVE_CNF 0X58

#define FRAME_POWER_FREQUENCY_CURRENT_REQ 0X5A
#define FRAME_POWER_FREQUENCY_CURRENT_RSP 0X5B
#define FRAME_POWER_FREQUENCY_CURRENT_RSP_DATA_LEN 0X0017

#define FRAME_VOLTAGE_WARNING_UPLOAD_REQ 0X5F
#define FRAME_VOLTAGE_WARNING_UPLOAD_CNF 0X60
#define FRAME_VOLTAGE_WARNING_CURVE_REQ 0X62
#define FRAME_VOLTAGE_WARNING_CURVE_RSP 0X63
#define FRAME_VOLTAGE_WARNING_CURVE_CNF 0X64

#define FRAME_ERROR_RETRANSMISSION_REQ  0x65

#define FRAME_NO_OVER_VOLTAGE_WARNING_LEN 0X0D
#define FRAME_OVER_VOLTAGE_CURVE_DATA_EXTRA_LEN 0X0D

#define FRAME_COLLECTOR_INFO_REQ 0X5C
#define FRAME_COLLECTOR_INFO_RSP 0X5D
#define FRAME_COLLECTOR_INFO_RSP_DATA_LEN 0X5F

#define FRAME_OVER_VOLTAGE_FLAG_REQ 0X5E
#define FRAME_ERROR_RSP 0x99

#define FRAME_HEART_BEAT_EVENT    0X66
#define FRAME_HEART_BEAT_EVENT_DATA_LEN    0X06

#define FRAME_TRANSFER_FILE_REQ   0X68
#define FRAME_TRANSFER_FILE_RSP   0X69
#define FRAME_TRANSFER_FILE_RSP_DATA_LEN   0X05

#define FRAME_UPGRADE_PROGRAME_REQ 0x6A
#define FRAME_UPGRADE_PROGRAME_RSP 0X6B
#define FRAME_UPGRADE_PROGRAME_RSP_DATA_LEN 0X03

#define FRAME_PD_CFG_SET_REQ 0x70
#define FRAME_PD_THRESHOLD_RSP 0X71
#define FRAME_PD_THRESHOLD_RSP_DATA_LEN 0X02

#define FRAME_SEARCH_PD_REQ 0x72
#define FRAME_SEARCH_PD_RSP 0X73
#define FRAME_SEARCH_PD_RSP_DATA_LEN 0X02

#define FRAME_PD_CURVE_REQ 0x74
#define FRAME_PD_CURVE_RSP 0X75
#define FRAME_PD_CURVE_RSP_DATA_LEN 0X02

#define FRAME_PRPD_REQ 0x76
#define FRAME_PRPD_RSP 0X77
#define FRAME_PRPD_RSP_DATA_LEN 0X02

#define FRAME_GET_DEVICE_VERSION 0xA5
#define FRAME_GET_DEVICE_VERSION_RSP 0xA6
#define FRAME_GET_DEVICE_VERSION_RSP_DATA_LEN 0x02

#define FRAME_SET_TIME 0xA7
#define FRAME_SET_DEVICE_TIME_RSP 0xA8
#define FRAME_SET_DEVIEC_CURRENT_TIME_RSP_DATA_LEN 0x02

#define FRAME_GET_TEMPERATURE 0xA9
#define FRAME_GET_TEMPERATURE_RSP 0xAA
#define FRAME_GET_TEMPERATURE_RSP_DATA_LEN 0X12

#define FRAME_REBOOT_FPGA_OR_ARM 0x80
#define FRAME_REBOOT_FPGA_OR_ARM_RSP 0x81
#define FRAME_REBOOT_RSP_DATA_LEN 0x03

#define FRAME_GET_CURRENT_PHASE_HARMONICS 0x8A
#define FRAME_GET_CURRENT_PHASE_HARMONICS_RSP 0X8B
#define FRAME_GET_CURRENT_PHASE_HARMONICS_RSP_DATA_LEN 0x106

typedef struct _header
{
   uint8_t starter1;
   uint16_t slave_addr;
   uint16_t master_addr;
   uint16_t data_len;
}header_t;

typedef struct _channel_cfg {
    uint8_t channel;
    uint8_t max_fre;
    uint8_t min_fre;
    uint8_t gain;
    uint16_t threshold;
}channel_cfg_t;

typedef struct _read_sample_data_req {
    uint8_t channel_count;
    uint32_t timestamp;
    channel_cfg_t cfg[MAX_CHANNEL];
}read_sample_data_req_t;

typedef struct _read_hf_current_jf_curve_req {
    uint8_t channel;
    uint32_t timestamp;
}read_hf_current_jf_curve_req_t;

typedef struct _read_dielectric_loss_curve_req {
    uint8_t channel;
    uint32_t timestamp;
}read_dielectric_loss_curve_req_t;

typedef struct _read_pf_current_data_req {
    uint8_t channel;
    uint32_t timestamp;
}read_pf_current_data_req_t;


typedef struct _read_current_phase_req_t{
   uint8_t channel;
   uint32_t timestamp;
}read_current_phase_req_t;

typedef struct _voltage_warning_upload_cnf {
    uint16_t count;
    uint8_t channel[3];
    uint32_t timestamp[3];
    uint16_t sn[3];
}over_voltage_warning_upload_cnf_t;

typedef struct _read_voltage_warning_curve_req {
    uint8_t channel;
    uint32_t timestamp;
    uint16_t sn;
}read_voltage_warning_curve_req_t;

typedef struct _read_collector_info_req{
    uint8_t channel;
}read_collector_info_req_t;

typedef struct _read_over_voltage_flag_req{
   uint8_t channel;
   uint16_t threshold[4];
}read_over_voltage_flag_req_t;

typedef struct _frame_error_retransmission_req{
    uint32_t timestamp;
    uint8_t reqtype;
    uint8_t channel;
    uint16_t errcnt;
    uint16_t sn;
    uint8_t errnum[128];
}frame_error_retransmission_req_t;

typedef struct _transfer_file_req_t {
    uint8_t file_type;
    uint16_t file_count;
    uint16_t file_index;
    uint8_t md5[32];
    uint16_t data_len;
    char buff[BUFFER_LEN];
} transfer_file_req_t;

typedef struct _frame_upgrade_programe_req {
   uint8_t file_type;
   uint8_t md5[33];
}frame_upgrade_programe_req_t;

typedef struct _frame_pd_threshold_req {
    uint8_t channel_count;
    uint8_t channel[3];
    uint16_t threshold[3];
    uint16_t change_rate[3];
    uint8_t mode[3];
}frame_pd_threshold_req_t;

typedef struct _frame_get_temperature_req_t
{
   uint32_t timestamp;

}frame_get_temperature_req_t;

typedef struct _frame_search_pd_req {
    uint32_t start_time;
    uint32_t end_time;
}frame_search_pd_req_t;

typedef struct _frame_pd_curve_req {
    uint8_t channel;
    uint32_t timestamp;
    uint32_t nanosecond;
}frame_pd_curve_req_t;

typedef struct _frame_prpd_req {
    uint8_t channel;
    uint32_t timestamp;
}frame_prpd_req_t;

typedef struct _set_device_time{
	uint32_t time;
}frame_set_time_t;

typedef struct _frame_reboot_type_t{
   uint8_t reboot_type;
}frame_reboot_type_t;

typedef struct _frame_req {
   header_t header;
   uint8_t func_code;
   union {
       read_sample_data_req_t read_sample_data_req;
       read_hf_current_jf_curve_req_t read_hf_current_jf_req;
       read_dielectric_loss_curve_req_t read_dielectric_loss_curve_req;
       read_pf_current_data_req_t read_pf_current_data_req;
       read_current_phase_req_t read_current_phase_req;
       read_collector_info_req_t  read_collector_info_req;
       read_over_voltage_flag_req_t read_over_voltage_flag_req;
       over_voltage_warning_upload_cnf_t voltage_warning_upload_cnf;
       read_voltage_warning_curve_req_t read_voltage_warning_curve_req;
       frame_error_retransmission_req_t frame_error_retransmission_req;
       transfer_file_req_t transfer_file_req;
       frame_upgrade_programe_req_t upgrade_programe_req;
       frame_get_temperature_req_t  get_temperature_req;
       frame_pd_threshold_req_t pd_threshold_req;
       frame_search_pd_req_t search_pd_req;
       frame_pd_curve_req_t pd_curve_req;
       frame_prpd_req_t prpd_req;
       frame_set_time_t device_time;
       frame_reboot_type_t reboot_type;
   } frame_req;
   uint8_t cs;
}frame_req_t;

void set_device_id(uint16_t id);
uint16_t get_device_id(void);
uint8_t byte_sum_checksum(uint8_t *data, uint32_t length);

#endif /* FRAME_H_ */
