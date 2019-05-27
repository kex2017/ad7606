#ifndef DATA_TRANSFER_H_
#define DATA_TRANSFER_H_
#include <stdint.h>
#include "kernel_types.h"
#include <stddef.h>
#include "ec20_at.h"
#include "thread.h"
#include "msg.h"
#include "log.h"

#define _MAX_PACKET_LEN 1024
#define PACKET_MSG_QUEUE_SIZE (16)
#define EC20_CSQ_UPDATE_TIME (1800)

typedef struct __packet_t{
    uint8_t data[_MAX_PACKET_LEN];
    uint32_t data_len;
}packet_t;

#define CACHE_NUM 4
typedef struct _cache{
    packet_t cache;
    uint8_t use_flag;
}cache_t;
uint8_t set_data_to_cache(uint8_t* cache_data, uint32_t cache_data_len);

void history_hook(kernel_pid_t pid );
void data_transfer_hook(kernel_pid_t pid );

uint8_t get_ec20_link_flag(void);
int get_ec20_csq_rssi(void);
uint16_t get_tcp_port(void);
void get_domain_name(char* buf);

uint16_t get_ec20_imei(char* imei);

void break_ec20_link(void);

kernel_pid_t data_transfer_init(void);

#endif /* SRC_DATA_TRANSFER_H_ */
