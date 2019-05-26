#include "cable_ground_service.h"

#include <string.h>
#include <stdio.h>
#include "log.h"
#include "comm.h"
#include "frame_decode.h"
#include "frame_handler.h"
#include "circular_task.h"
#include "comm_packet.h"
#include "msg.h"
#include "cable_ground_threads.h"

#define RCV_QUEUE_SIZE  (8)

static msg_t rcv_queue[RCV_QUEUE_SIZE];
void *cable_ground_service(void *argv)
{
    (void)argv;

    uint8_t frame_data[512] = { 0 };
    frame_req_t req_data = { 0 };

    comm_init();
    msg_t msg;
    PACKET *packet;
    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);

    while (1) {
        msg_receive(&msg);
        packet = (PACKET*)(msg.content.ptr);
        memcpy(frame_data, packet->data, packet->data_len);
        LOG_DUMP((const char*)packet->data, packet->data_len);
        frame_decode(frame_data, &req_data);
        frame_handler(&req_data);
        memset(frame_data, 0, sizeof(frame_data));
    }

    return 0;
}


kernel_pid_t cable_ground_service_pid;
char cable_ground_service_thread_stack[THREAD_STACKSIZE_MAIN*8] __attribute__((section(".big_data")));
kernel_pid_t cable_ground_service_init(void)
{
	cable_ground_service_pid = thread_create(cable_ground_service_thread_stack,
			sizeof(cable_ground_service_thread_stack), CABLE_GROUND_PRIORITY,
			THREAD_CREATE_STACKTEST, cable_ground_service, NULL,
			"cable_ground_srv");
   return cable_ground_service_pid;
}

