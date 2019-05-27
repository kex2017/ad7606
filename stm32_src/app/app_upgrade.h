#ifndef APP_UPGRADE_H_
#define APP_UPGRADE_H_

#include "frame_common.h"

typedef struct _miss_pack_t{
	uint32_t file_count;
	uint8_t bit_filed[100];
}recv_pack_t;

int process_upload_file_req(transfer_file_req_t *file_info, uint32_t file_index, uint8_t *last_packet);
void process_upgrade_programe_req( uint8_t file_type);

recv_pack_t get_recv_pack_info(void);
void add_recv_pkt_info(uint32_t file_count, uint32_t file_index);

int get_is_upgrade_flag(void);

void set_is_upgrade_flag(int value);

void clear_recv_file_info(void);

uint8_t check_file_info_integrity(uint32_t count);

void set_arm_file_transfer_flag(uint32_t flag);
uint32_t get_upgrade_arm_flag(void);
uint32_t is_arm_file_transfer_timeout(void);

#endif
