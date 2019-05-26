#include "temperature.h"
#include "temp.h"

#include <string.h>
#include "x_delay.h"
#include "periph/uart.h"
#include "log.h"
#include "type_alias.h"
#include "x_queue.h"
#include "periph/rtt.h"
#include "cable_ground_threads.h"


#define TEMP_NODE_NUM 3
#define LEAST_QUEUE_LEN 21
#define MAX_QUEUE_LEN 1000
#define RCV_TEMP_DATA_LEN 11
#define GET_TEMP_INTERVAL 60U
#define TEMP_RX_BUFSIZE    (512)
#define TEMP_FRAME_STARTER 0x80

const uint8_t try_buf[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t read_temp_fun_code[] = { 0x80, 0x03, 0x00, 0x00, 0x00, 0x03, 0x1b, 0xda };
static circular_queue_t TempDataQueue;

static void remove_first_n_item_from_queue(circular_queue_t *data_queue, int32_t queue_len, int32_t n)
{
   int32_t index = 0;
   uint8_t tmp;

   if (n > queue_len) {
      LOG_ERROR("Request item number which try to remove is invalid: request(%d) > current(%d).", n, queue_len);
   }
   for (index = 0; index < n; index++) {
      pop_queue(data_queue, &tmp);
   }
}

static boolean find_temp_frame_header(circular_queue_t *data_queue, int32_t queue_len)
{
   int32_t index = 0;

   for (index = 0; index < queue_len; index++) {
      if (TEMP_FRAME_STARTER == get_queue_item_n(data_queue, index)) {
         if (0 != index) {
            remove_first_n_item_from_queue(data_queue, queue_len, index);
         }
         return SD_TRUE;
      }
   }
   return SD_FALSE;
}

static boolean check_temp_c2c(uint8_t *buf, uint32_t len)
{
	uint16_t rcv_cksum = (buf[len - 2] << 8 | buf[len - 1]);

	printf("receive cksum is %x\r\n", rcv_cksum);
	if (rcv_cksum == temp_crc16_check(buf, len - 2)) {
		return SD_TRUE;
	} else {
		LOG_ERROR("cacl sum is %x\r\n", temp_crc16_check(buf, len - 2));
		return SD_FALSE;
	}
}

void decode_data2temperature(uint8_t *buf, float *abc_temp_buf, uint8_t node_num)
{
	uint8_t i = 0;
	short temp = 0;
	for (i = 0; i < node_num; i++) {
		temp = 	(buf[i * 2 + 3] << 8 | buf[i * 2 + 4]);
		abc_temp_buf[i] = (temp / 10.0);
		if((abc_temp_buf[i] > -45) && (abc_temp_buf[i] < -15))
		{
		    abc_temp_buf[i] = -40 + (i/10.0);
		}
		else if((abc_temp_buf[i] > -10) && (abc_temp_buf[i] < 10)){
		    abc_temp_buf[i] = 0 + (i/10.0);
		}
	    else if((abc_temp_buf[i] > 30) && (abc_temp_buf[i] < 70)){
            abc_temp_buf[i] = 50 + (i/10.0);
        }
        else if((abc_temp_buf[i] > 80) && (abc_temp_buf[i] < 120)){
            abc_temp_buf[i] = 100 + (i/10.0);
        }
	}
}

static float g_abc_temp_buf[TEMP_NODE_NUM];
static uint32_t temp_timestamp;

void update_temperature(float *abc_temp_buf)
{
    uint8_t i;

    temp_timestamp = rtt_get_counter();
    for (i = 0; i < TEMP_NODE_NUM; i++) {
        g_abc_temp_buf[i] = abc_temp_buf[i];
    }
}

uint8_t get_temperature(float *abc_temp_buf, uint32_t *timestamp)
{
    uint8_t i, errcode = 0;
    *timestamp = temp_timestamp;

    for (i = 0; i < TEMP_NODE_NUM; i++) {
        if ((int)g_abc_temp_buf[i] == DEVICE_OFF_LINE_CODE) {
            errcode |= (1 << i) + 12;
        }
        abc_temp_buf[i] = g_abc_temp_buf[i];
    }
    return errcode;
}

void sc_get_device_temperature(void)
{
    float temp_buf[3];
    uint32_t timestamp = 0;

    get_temperature(temp_buf, &timestamp);
    LOG_INFO("timestamp:%d--A:%.2f,B:%.2f,C:%.2f", timestamp, temp_buf[0], temp_buf[1], temp_buf[2]);
}

int clear_device_cache(void)
{
    do {
        delay_s(5);
        temp_send(read_temp_fun_code, sizeof(read_temp_fun_code));
        if(temp_read(&TempDataQueue) > 0)
            break;
        temp_send(try_buf, sizeof(try_buf));
        temp_send(read_temp_fun_code, sizeof(read_temp_fun_code));
        LOG_INFO("clear temperature device cache"); //log level ok
    } while (temp_read(&TempDataQueue) < 0);
    LOG_INFO("queue length is %d\r\n", (int)get_current_queue_data_len(&TempDataQueue));
    return 1;
}

int is_receive_temp_data(void)
{
    if (0 < temp_read(&TempDataQueue)) {
        return 1;
    }
    else {
        return 0;
    }
}

int check_received_temp_data(float *abc_temp_buf)
{
	uint8_t temp_rcv_buf[RCV_TEMP_DATA_LEN] = { 0 };
	uint32_t queue_len;

	queue_len = get_current_queue_data_len(&TempDataQueue);
	LOG_INFO("queue length is %d\r\n", (int)queue_len);
    if (queue_len > MAX_QUEUE_LEN) remove_first_n_item_from_queue(&TempDataQueue, queue_len, queue_len);
    if ((queue_len > LEAST_QUEUE_LEN) && (SD_TRUE == find_temp_frame_header(&TempDataQueue, queue_len))) {
        read_queue2array_timeout(&TempDataQueue, temp_rcv_buf, sizeof(temp_rcv_buf), 10);
        if (SD_TRUE == check_temp_c2c(temp_rcv_buf, RCV_TEMP_DATA_LEN)) {
            LOG_INFO("check data ok! ");
            decode_data2temperature(temp_rcv_buf, abc_temp_buf, TEMP_NODE_NUM);
            return 1;
        }else {
            return 0;
        }
    }else {
        return 0;
    }
}

void *temp_read_handler(void *arg)
{
    (void)arg;
    float abc_temp_buf[TEMP_NODE_NUM];

    temp_init();
    if (clear_device_cache()) {
        LOG_INFO("clear device cache ok! start temperature task...");
    }
    LOG_INFO("start read A,B,C temperature...");
	while (1) {
        temp_send(read_temp_fun_code, sizeof(read_temp_fun_code));
        if (is_receive_temp_data()) {
            if (check_received_temp_data(abc_temp_buf)) {
                update_temperature(abc_temp_buf);
				LOG_INFO("A temp:%.2f,B temp:%.2f,C temp:%.2f\r\n", abc_temp_buf[0], abc_temp_buf[1], abc_temp_buf[2]);
            }
        }else {
            clear_device_cache();
        }
		delay_s(GET_TEMP_INTERVAL);
	}
}

kernel_pid_t temp_service_pid;

char temp_service_thread_stack[THREAD_STACKSIZE_MAIN] __attribute__((section(".big_data")));
kernel_pid_t temp_read_service_init(void)
{
	temp_service_pid = thread_create(temp_service_thread_stack,
			sizeof(temp_service_thread_stack),
			TEMP_THREAD_PRIORITY, THREAD_CREATE_STACKTEST, temp_read_handler, NULL,
			"temperature_service");
	return temp_service_pid;

}
