#include "data_transfer.h"
#include "periph/rtt.h"
#include "frame_paser.h"
#include "frame_common.h"
#include "frame_handler.h"
#include "timex.h"
#include "xtimer.h"
#include "log.h"
#include "dev_cfg.h"
#define ENABLE_DEBUG (1)
#include "debug.h"

static mutex_t cache_mtx;
static char _rx_buf_mem[EC20_RX_BUFFSIZE];
static int p_rssi = 0;

static ec20_dev_t dev;
static frame_paser_dev_t fp_dev;

static kernel_pid_t history_send_pid;
static msg_t disconnect_msg;

static void delay_ms(int ms)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}

void get_domain_name(char* buf)
{
    strncpy(buf, dev.ip, sizeof(dev.ip));
}

uint16_t get_tcp_port(void)
{
    return dev.port;
}

void history_hook(kernel_pid_t pid)
{
	history_send_pid = pid;
}

void set_server_ip_port(char *server_ip, uint16_t port)
{
	server_info_t *server_info = get_dev_server_info();

	for(int i = 0 ; i<4; i++)
	{
		dev.ip[i] = server_ip[i];
		server_info->ip[i] =  server_ip[i];
	}
    dev.port = port;
    server_info->port = port;

    printf("set ip port %d, ip %d.%d.%d.%d",port, server_ip[0], server_ip[1], server_ip[2], server_ip[3]);
    printf("2.set ip port %d, ip %d.%d.%d.%d",port, dev.ip[0], dev.ip[1], dev.ip[2], dev.ip[3]);
    break_ec20_link();
}

void break_ec20_link(void)
{
    dev.link_status = LINK_BREAK;
}

uint8_t get_ec20_link_flag(void)
{
    return dev.link_status;
}

uint16_t get_ec20_imei(char* imei)
{
    strncpy(imei, dev.imei, EC20_SIM_IMEI_MAX_LEN);
    return strlen(imei);
}

int test_at(char* at_cmd, char* out)
{
    return test_at_cmd(&dev, at_cmd, out);
}

at_cfg_t test_at_cfg = {
       .uart = EC20_UART_DEV,
       .baudrate = UART_BAUDRATE,
       .isrpipe_buf = _rx_buf_mem,
       .isrpipe_buf_size = EC20_RX_BUFFSIZE,
       .set_highest_priority = 1,
};

int get_ec20_csq_rssi(void)
{
    return p_rssi;
}

void interval_update_csq(ec20_dev_t* dev, int* rssi)
{
    static uint32_t csq_now, csq_start;
    csq_now = rtt_get_counter();
    if ((csq_now - csq_start) > EC20_CSQ_UPDATE_TIME) {
        if (!ec20_4g_csq(dev)) {
            return;
        }
        else {
            *rssi = dev->csq.rssi;
        }
        csq_start = csq_now;
    }
}

void acquire_cache_mtx(void)
{
    mutex_lock(&cache_mtx);
}

void release_cache_mtx(void)
{
    mutex_unlock(&cache_mtx);
}

cache_t cache_buf[CACHE_NUM];

uint8_t set_data_to_cache_by_num(uint8_t* cache_data, uint32_t cache_data_len, uint8_t cache_num)
{
    memset(&cache_buf[cache_num].cache, 0, sizeof(cache_buf[cache_num].cache));
    memcpy(cache_buf[cache_num].cache.data, cache_data, cache_data_len);
    cache_buf[cache_num].cache.data_len = cache_data_len;
    return 0;
}

uint8_t set_data_to_cache(uint8_t* cache_data, uint32_t cache_data_len)
{
    acquire_cache_mtx();
    for(uint8_t i = 0; i < CACHE_NUM; i++){
        if(cache_buf[i].use_flag == 0){
            set_data_to_cache_by_num(cache_data, cache_data_len, i);
            LOG_DEBUG("set data to cache %d OK", i);
            cache_buf[i].use_flag = 1;
            release_cache_mtx();
            return 1;
        }
    }
    release_cache_mtx();
    return 0;
}

uint8_t get_data_from_cache(packet_t* cache_data)
{
    acquire_cache_mtx();
    for (uint8_t i = 0; i < CACHE_NUM; i++) {
        if (cache_buf[i].use_flag) {
            *cache_data = cache_buf[i].cache;
            cache_buf[i].use_flag = 0;
            LOG_DEBUG("get cache %d data OK！", i);
            release_cache_mtx();
            return 1;
        }
    }
    release_cache_mtx();
    return 0;
}

int is_dev_info_by_cmd_id(uint8_t *cmd_id)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (strncmp((char*)cmd_id, (char*)get_dev_cur_info_by_phase(i)->cmd_id, ID_LEN)==0) {
        	return 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (strncmp((char*)cmd_id, (char*)get_dev_vol_info_by_phase(i)->cmd_id, ID_LEN)==0) {
        	return 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (strncmp((char*)cmd_id, (char*)get_dev_hfct_info_by_phase(i)->cmd_id, ID_LEN)==0) {
        	return 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (strncmp((char*)cmd_id, (char*)get_dev_sc_alarm_info_by_phase(i)->cmd_id, ID_LEN)==0) {
        	return 0;
        }
    }
    if (strncmp((char*)cmd_id, (char*)get_dev_alarm_info()->cmd_id, ID_LEN)==0) {
    	return 0;
    }
    LOG_INFO("Receive error cmd id");
    return -1;
}

uint8_t send_data[512] = {0};
packet_t recv_pkt, packet, send_pkt;

uint8_t break_times = 0;
uint8_t link_up_flag = 0, first_link_break_flag = 0;
void *data_transfer_service(void *arg)
{
    (void)arg;
    LOG_INFO("data transfer start");
    ec20_at_setup(&dev, &test_at_cfg, EC20_RESET_PIN);

    while (1) {
        if (LINK_BREAK == ec20_get_link_status(&dev)) {
            disconnect_msg.type = BREAK;
            msg_send(&disconnect_msg,history_send_pid);
            get_dev_ip_port();
            if (!ec20_link_up(&dev)) {
                DEBUG("ec20 link failed\r\n");
                delay_ms(300);
                continue;
            }
            disconnect_msg.type = LINK;
            msg_send(&disconnect_msg,history_send_pid);
        }
        if (LINK_UP == ec20_get_link_status(&dev)) {
        	if( !is_frame_parser_busy(&fp_dev)){
            recv_pkt.data_len = ec20_at_recv(&dev, recv_pkt.data);
            if (recv_pkt.data_len ) {
            	add_frame_data_stream(&fp_dev, recv_pkt.data, recv_pkt.data_len);
            	}
        	}
            for(uint8_t i = 0; i < CACHE_NUM; i++){
                if(cache_buf[i].use_flag){
                    send_pkt = cache_buf[i].cache;
                    if(send_pkt.data_len > 0 && ec20_at_send(&dev, send_pkt.data, send_pkt.data_len)<0)
                    {
                    	LOG_ERROR("EC20 send data error");
                    	break;
                    }
                    cache_buf[i].use_flag = 0;
                }
            }
            interval_update_csq(&dev, &p_rssi);
        }
        delay_ms(100);
    }
}

#define DATA_TRANSFER_PRIO (4)
kernel_pid_t data_transfer_pid = KERNEL_PID_UNDEF;
#define DATA_TRANSFER_STACKSIZE          (THREAD_STACKSIZE_MAIN)
static char data_transfer_stack[DATA_TRANSFER_STACKSIZE*3];
kernel_pid_t data_transfer_init(void)
{
    if (data_transfer_pid == KERNEL_PID_UNDEF) {
        data_transfer_pid = thread_create(data_transfer_stack,
                sizeof(data_transfer_stack),
                DATA_TRANSFER_PRIO, THREAD_CREATE_STACKTEST,
                data_transfer_service, NULL, "data transfer service");
    }
    return data_transfer_pid;
}
/* Data transfer thread end */

/* Frame parser data thread start */
#define FRAME_PARSER_DATA_LEN (1024)
#define HEAD_LIST_LEN (2)
#define FRAME_INFO_DATA_LEN (17 + 1 + 1)
char rx_buff[FRAME_PARSER_DATA_LEN] = {0};
uint8_t parser_buff[FRAME_PARSER_DATA_LEN] = {0};
uint8_t head[HEAD_LIST_LEN] = {0xA5, 0x5A};

int checksum(uint8_t* data,uint16_t data_len,uint8_t *crc_buf,uint16_t crc_len)
{
	(void )crc_len;
    uint16_t check_sum = 0;

    check_sum = RTU_CRC(data, data_len);
    uint8_t *p = (uint8_t *)&check_sum;
    crc_buf[0] = p[0];
    crc_buf[1] = p[1];

    return sizeof(uint16_t);
}

void *frame_parser_data(void *arg)
{
	(void)arg;
	frame_req_t req_data = { 0 };
	int rev_data_len= 0;
	if(0 > frame_paser_init(&fp_dev, rx_buff, FRAME_PARSER_DATA_LEN, head, HEAD_LIST_LEN, checksum, HEAD_LIST_LEN, FRAME_INFO_DATA_LEN))
	{
		LOG_INFO("Frame parser init error");
	}

	while(1)
	{
		delay_ms(100);
		if((rev_data_len = do_frame_parser(&fp_dev, parser_buff, FRAME_PARSER_DATA_LEN)) > 0 )
		{
			frame_decode(parser_buff, &req_data);
			if(is_dev_info_by_cmd_id(req_data.header.cmd_id) == 0)
			{
				frame_handler(&req_data);
			}
			memset(parser_buff, 0, rev_data_len);
		}
	}
	return NULL;
}

#define FRAME_PARSER_PRIO (5)
kernel_pid_t frame_parser_data_pid = KERNEL_PID_UNDEF;
static char frame_parser_data_stack[DATA_TRANSFER_STACKSIZE*2];
kernel_pid_t frame_parser_data_init(void)
{
    if (frame_parser_data_pid == KERNEL_PID_UNDEF) {
    	frame_parser_data_pid = thread_create(frame_parser_data_stack,
                sizeof(frame_parser_data_stack),
				FRAME_PARSER_PRIO, THREAD_CREATE_STACKTEST,
                frame_parser_data, NULL, "frame parser service");
    }
    return frame_parser_data_pid;
}
/* Frame parser data thread end*/
