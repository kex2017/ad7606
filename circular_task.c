#include <string.h>

#include "circular_task.h"
#include "frame_handler.h"

#include "frame_encode.h"
#include "data_acquisition.h"
#include "over_voltage.h"
#include "log.h"
#include "comm.h"
#include "thread.h"
#include "x_delay.h"
#include "comm_packet.h"
#include "heart_beat.h"
#include "periph/rtt.h"
#include "partial_discharge.h"
#include "cable_ground_threads.h"

msg_t msg[6] = { 0 };
static PACKET circular_packet[5] = { 0 };
circular_task_t circular_task[5] = { 0 };

static uint8_t hf_current_jf_buffer[MAX_FPGA_DATA_LEN] __attribute__((section(".big_data")));
static uint8_t dielectric_loss_buffer[MAX_FPGA_DATA_LEN] __attribute__((section(".big_data")));
static uint8_t over_voltage_buffer[MAX_FPGA_DATA_LEN] __attribute__((section(".big_data")));
static uint8_t partial_discharge_buffer[MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN] __attribute__((section(".big_data")));
static uint8_t prpd_data_buffer[MAX_PRPD_DATA_LEN] __attribute__((section(".big_data")));

static kernel_pid_t sender_pid;
void comm_circular_sender_hook(kernel_pid_t pid)
{
	sender_pid = pid;
}

void msg_circular_packet_send(uint8_t *data, uint32_t data_len, TaskType type)
{
    msg_t *m = NULL;
    PACKET *packet = NULL;

    if (HF_CURRENT_JF_CURVE == type || HF_CURRENT_JF_CURVE_ERR == type) {
        m = &msg[0];
        packet = &circular_packet[0];
    }
    if (DIELECTRIC_LOSS_CURVE == type || DIELECTRIC_LOSS_CURVE_ERR == type) {
        m = &msg[1];
        packet = &circular_packet[1];
    }
    if (VOLTAGE_WARNING_CURVE == type || VOLTAGE_WARNING_CURVE_ERR == type) {
        m = &msg[2];
        packet = &circular_packet[2];
    }
    if (PARTIAL_DISCHARGE_CURVE == type || PARTIAL_DISCHARGE_CURVE_ERR == type) {
        m = &msg[3];
        packet = &circular_packet[3];
    }
    if (PRPD_DATA == type) {
        m = &msg[4];
        packet = &circular_packet[4];
    }
    if (HEART_BEAT == type) {
        m = &msg[5];
        packet = &circular_packet[5];
    }

    packet->data_len = data_len;
	memcpy(packet->data, data, data_len);
	m->content.ptr = (void *) packet;
	msg_send(m, sender_pid);
}

circular_task_t *get_task_pointer(TaskType type)
{
    if (HF_CURRENT_JF_CURVE == type || HF_CURRENT_JF_CURVE_ERR == type) {
        return &circular_task[0];
    }
    if (DIELECTRIC_LOSS_CURVE == type || DIELECTRIC_LOSS_CURVE_ERR == type) {
        return &circular_task[1];
    }
    if (VOLTAGE_WARNING_CURVE == type || VOLTAGE_WARNING_CURVE_ERR == type) {
        return &circular_task[2];
    }
    if (PARTIAL_DISCHARGE_CURVE == type || PARTIAL_DISCHARGE_CURVE_ERR == type) {
        return &circular_task[3];
    }
    if (PRPD_DATA == type || PRPD_DATA_ERR == type) {
        return &circular_task[4];
    }

    return NULL;
}

static uint8_t last_partial_discharge_task_channel = 2;
uint8_t get_last_partial_discharge_task_channel(void)
{
	return last_partial_discharge_task_channel;
}

void set_last_partial_discharge_task_channel(uint8_t channel)
{
	last_partial_discharge_task_channel = channel;
}

boolean load_circular_task_curve_data(uint8_t channel, TaskType type, uint32_t timestamp, uint16_t sn)
{
    uint16_t total_packet = 0;
    if (HF_CURRENT_JF_CURVE == type) {
        if(0 > data_acquisition_read_curve_data(channel, timestamp, MAX_FPGA_DATA_LEN, 0, hf_current_jf_buffer, &total_packet))
        {
            return SD_FALSE;
        }
    }
    if (DIELECTRIC_LOSS_CURVE == type) {
        if(0 > data_acquisition_read_curve_data(channel, timestamp, MAX_FPGA_DATA_LEN, 0, dielectric_loss_buffer, &total_packet))
        {
            return SD_FALSE;
        }
    }
    if (VOLTAGE_WARNING_CURVE == type) {
        if(0 > over_voltage_get_curve_data(channel, timestamp, sn, 0, &total_packet, over_voltage_buffer, MAX_FPGA_DATA_LEN))
        {
            return SD_FALSE;
        }
    }
    if (PARTIAL_DISCHARGE_CURVE == type) {
        if(0 > partial_discharge_get_curve_data(channel, sn, MAX_PARTIAL_DISCHARGE_CURVE_DATA_LEN, 0, partial_discharge_buffer))
        {
        	set_last_partial_discharge_task_channel(channel);
            return SD_FALSE;
        }
    }
    if (PRPD_DATA == type) {
       if(0 > prpd_get_data(channel, MAX_FPGA_DATA_LEN, 0, prpd_data_buffer))
       {
           return SD_FALSE;
       }
    }

    return SD_TRUE;
}

boolean set_circular_task_base(uint8_t channel, TaskType type, uint32_t data_len, uint16_t total_packet, uint16_t *list_packet, uint16_t list_count, uint32_t timestamp, uint16_t sn)
{
    circular_task_t *task = NULL;
    if(data_len == 0 || total_packet == 0) return SD_FALSE;

    task = get_task_pointer(type);
    if(NULL == task) return SD_FALSE;

    if(SD_FALSE == load_circular_task_curve_data(channel, type, timestamp, sn)) return SD_FALSE;

    if (HF_CURRENT_JF_CURVE == type || DIELECTRIC_LOSS_CURVE == type || VOLTAGE_WARNING_CURVE == type || PARTIAL_DISCHARGE_CURVE == type || PRPD_DATA == type) {
        task->flag = ON;
        task->type = type;
        task->channel = channel;
        task->cur_packet = list_packet[0];
        task->list_count = list_count;
        task->total_packet = total_packet;
        task->sent_count = 0;
        task->packet_len = 0;
        task->data_len = data_len;
        task->timestamp = timestamp;
        task->sn = sn;
    }

    if (HF_CURRENT_JF_CURVE_ERR == type || DIELECTRIC_LOSS_CURVE_ERR == type || VOLTAGE_WARNING_CURVE_ERR == type) {
        task->flag = ON;
        task->type = type;
        task->channel = channel;
        task->cur_packet = list_packet[0];
        task->list_count = list_count;
        task->sent_count = 0;
        task->packet_len = 0;
        task->timestamp = timestamp;
        task->sn = sn;
    }

    for(uint16_t i = 0; i < list_count; i++)
    {
        task->list_packet[i] = list_packet[i];
    }

    return SD_TRUE;
}

void clear_circular_task_base(TaskType type)
{
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    task->flag = OFF;
    task->type = EMPTY_CURVE;
    task->channel = 0;
    task->cur_packet = 0;
    task->list_count = 0;
    task->sent_count = 0;
    task->packet_len = 0;
    task->timestamp = 0;
    task->sn = 0;
    memset(task->list_packet, 0x0, sizeof(task->list_packet));
    LOG_INFO("Clear TaskType:%d task.", type);
}

void update_circular_task_base(TaskType type)
{
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;


    if (ON == task->flag) {
        task->sent_count++;
        if (task->sent_count == task->list_count) {
            clear_circular_task_base(type);
            return;
        }
        task->cur_packet = task->list_packet[task->sent_count];
    }
}

boolean is_have_task_if_need(TaskType type)
{
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return SD_FALSE;

    if(ON == task->flag)
    {
        return SD_TRUE;
    }

    return SD_FALSE;
}

void calc_cur_packet_len(TaskType type)
{
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    if (task->cur_packet == task->total_packet - 1) {
        if (0 != task->data_len % MAX_RSP_DATA_LEN) {
            task->packet_len = task->data_len % MAX_RSP_DATA_LEN;
        }
        else {
            task->packet_len = MAX_RSP_DATA_LEN;
        }
    }
    else {
        task->packet_len = MAX_RSP_DATA_LEN;
    }
}

void get_circular_task_curve_packet(uint8_t *packet_data, uint16_t cur_packet, uint16_t packet_len, TaskType type)
{
    uint32_t addr = cur_packet * MAX_RSP_DATA_LEN;
    if (HF_CURRENT_JF_CURVE == type || HF_CURRENT_JF_CURVE_ERR == type) {
        memcpy(packet_data, hf_current_jf_buffer + addr, packet_len);
    }
    if (DIELECTRIC_LOSS_CURVE == type || DIELECTRIC_LOSS_CURVE_ERR == type) {
        memcpy(packet_data, dielectric_loss_buffer + addr, packet_len);
    }
    if (VOLTAGE_WARNING_CURVE == type || VOLTAGE_WARNING_CURVE_ERR == type) {
        memcpy(packet_data, over_voltage_buffer + addr, packet_len);
    }
    if (PARTIAL_DISCHARGE_CURVE == type || PARTIAL_DISCHARGE_CURVE_ERR == type) {
        memcpy(packet_data, partial_discharge_buffer + addr, packet_len);
    }
    if (PRPD_DATA == type || PRPD_DATA_ERR == type) {
        memcpy(packet_data, prpd_data_buffer + addr, packet_len);
    }
}

void hf_current_jf_curve_task(TaskType type)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    calc_cur_packet_len(type);

    get_circular_task_curve_packet(packet_data, task->cur_packet, task->packet_len, type);
    length = frame_hf_current_jf_curve_encode(data, packet_data, task->packet_len, task->cur_packet, task->total_packet, DEVICEOK, task->channel, task->timestamp);
    LOG_INFO("Send curve hf current jf: length:%d cur_packet:%d total_packet:%d packet_len:%d",length,task->cur_packet,task->total_packet,task->packet_len);
    msg_circular_packet_send(data, length, type);
    update_circular_task_base(type);
}

void dielectric_loss_curve_task(TaskType type)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    calc_cur_packet_len(type);

    get_circular_task_curve_packet(packet_data, task->cur_packet, task->packet_len, type);
    length = frame_dielectric_loss_curve_encode(data, packet_data, task->packet_len, task->cur_packet, task->total_packet, DEVICEOK, task->channel, task->timestamp);
    LOG_INFO("Send curve dielectric loss: length:%d cur_packet:%d total_packet:%d packet_len:%d",length,task->cur_packet,task->total_packet,task->packet_len);
    msg_circular_packet_send(data, length, type);
    update_circular_task_base(type);
}


void voltage_warning_curve_task(TaskType type)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    calc_cur_packet_len(type);

    get_circular_task_curve_packet(packet_data, task->cur_packet, task->packet_len, type);
    length = frame_over_voltage_curve_encode(data, packet_data, task->packet_len, task->cur_packet, task->total_packet, DEVICEOK, task->channel, task->timestamp, task->sn);
    LOG_INFO("Send curve over voltage: length:%d cur_packet:%d total_packet:%d packet_len:%d",length,task->cur_packet,task->total_packet,task->packet_len);
    msg_circular_packet_send(data, length, type);
    update_circular_task_base(type);
}

void partial_discharge_curve_task(TaskType type)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    calc_cur_packet_len(type);

    get_circular_task_curve_packet(packet_data, task->cur_packet, task->packet_len, type);
    length = frame_partial_discharge_curve_encode(data, packet_data, task->packet_len, task->cur_packet, task->total_packet, DEVICEOK, task->channel, task->timestamp);
    LOG_INFO("Send curve partial discharge: length:%d cur_packet:%d total_packet:%d packet_len:%d",length,task->cur_packet,task->total_packet,task->packet_len);
    msg_circular_packet_send(data, length, type);
    update_circular_task_base(type);
}

void prpd_curve_task(TaskType type)
{
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    uint16_t length = 0;
    uint8_t packet_data[MAX_RSP_DATA_LEN] = { 0 };
    circular_task_t *task = NULL;

    task = get_task_pointer(type);
    if(NULL == task) return;

    calc_cur_packet_len(type);

    get_circular_task_curve_packet(packet_data, task->cur_packet, task->packet_len, type);
    length = frame_prpd_data_encode(data, packet_data, task->packet_len, task->cur_packet, task->total_packet, DEVICEOK, task->channel, get_g_last_sample_time(), get_g_this_sample_time());
    LOG_INFO("Send channel %d prpd data: length:%d cur:%d total:%d len:%d",task->channel, length,task->cur_packet,task->total_packet,task->packet_len);
    msg_circular_packet_send(data, length, type);
    update_circular_task_base(type);
}

void *circular_task_service(void *arg)
{
    (void)arg;
    while (1) {
        delay_ms(100);
        //dl
        if (SD_TRUE == is_have_task_if_need(circular_task[1].type)) {
            dielectric_loss_curve_task(circular_task[1].type);
            continue;
        }

        //jf
        if (SD_TRUE == is_have_task_if_need(circular_task[0].type)) {
            hf_current_jf_curve_task(circular_task[0].type);
            continue;
        }

        //ov
        if (SD_TRUE == is_have_task_if_need(circular_task[2].type)) {
            voltage_warning_curve_task(circular_task[2].type);
            continue;
        }

        //pd
        if (SD_TRUE == is_have_task_if_need(circular_task[3].type)) {
            partial_discharge_curve_task(circular_task[3].type);
            continue;
        }

        //prpd
        if (SD_TRUE == is_have_task_if_need(circular_task[4].type)) {
            prpd_curve_task(circular_task[4].type);
            continue;
        }
    }
    return NULL;
}

char circular_task_thread_stack[THREAD_STACKSIZE_MAIN * 28] __attribute__((section(".big_data")));
kernel_pid_t circular_task_service_pid;
void *heartbeat_service(void *arg)
{
    (void)arg;
    memset(circular_task, 0x0, sizeof(circular_task));

    if (circular_task_service_pid == KERNEL_PID_UNDEF) {
        circular_task_service_pid = thread_create(circular_task_thread_stack,
                                                               sizeof(circular_task_thread_stack),
                                                               CIRCULAR_TASK_THREAD_PRIORITY,
                                                               THREAD_CREATE_STACKTEST,
                                                               circular_task_service, NULL, "ct_service");
    }

    while (1) {
        delay_ms(100);
        heart_beat_service();
    }
    return NULL;
}



char heartbeat_thread_stack[THREAD_STACKSIZE_MAIN*2] __attribute__((section(".big_data")));
kernel_pid_t heartbeat_thread_pid;
kernel_pid_t heartbeat_service_init(void)
{
	if (heartbeat_thread_pid == KERNEL_PID_UNDEF) {
		heartbeat_thread_pid = thread_create(heartbeat_thread_stack,
		                       sizeof(heartbeat_thread_stack),
		                       HEARTBEAT_THREAD_PRIORITY, THREAD_CREATE_STACKTEST, heartbeat_service, NULL,"heartbeat");
	}
	return heartbeat_thread_pid;
}
