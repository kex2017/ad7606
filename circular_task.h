#ifndef CIRCULAR_TASK_H_
#define CIRCULAR_TASK_H_

#include "frame_common.h"
#include "kernel_types.h"


typedef enum {EMPTY_CURVE, HF_CURRENT_JF_CURVE, DIELECTRIC_LOSS_CURVE, VOLTAGE_WARNING_CURVE, PARTIAL_DISCHARGE_CURVE,PRPD_DATA, HEART_BEAT, HF_CURRENT_JF_CURVE_ERR, DIELECTRIC_LOSS_CURVE_ERR, VOLTAGE_WARNING_CURVE_ERR, PARTIAL_DISCHARGE_CURVE_ERR, PRPD_DATA_ERR}TaskType;
typedef enum {OFF, ON}TaskFlag;

struct _circular_task {
    TaskFlag flag;
    TaskType type;
    uint8_t channel;
    uint16_t list_packet[512];
    uint16_t list_count;
    uint16_t cur_packet;
    uint16_t total_packet;
    uint16_t sent_count;
    uint16_t packet_len;
    uint32_t data_len;
    uint32_t timestamp;
    uint16_t sn;
};

typedef struct _circular_task circular_task_t;

boolean is_have_task_if_need(TaskType type);
uint8_t get_last_partial_discharge_task_channel(void);
void comm_circular_sender_hook(kernel_pid_t pid);
kernel_pid_t heartbeat_service_init(void);
void msg_circular_packet_send(uint8_t *data, uint32_t data_len, TaskType type);
boolean set_circular_task_base(uint8_t channel, TaskType type,uint32_t data_len, uint16_t total_packet, uint16_t *list_packet, uint16_t list_count, uint32_t timestamp, uint16_t sn);

#endif
