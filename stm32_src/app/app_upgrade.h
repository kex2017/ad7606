#ifndef APP_UPGRADE_H_
#define APP_UPGRADE_H_

#include "frame_common.h"

int process_upload_file_req(transfer_file_req_t *file_info, uint8_t *last_packet);
void process_upgrade_programe_req(uint8_t *md5, uint8_t file_type);


void set_arm_file_transfer_flag(uint32_t flag);
uint32_t get_upgrade_arm_flag(void);
uint32_t is_arm_file_transfer_timeout(void);

#endif
