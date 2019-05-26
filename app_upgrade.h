#ifndef APP_UPGRADE_H_
#define APP_UPGRADE_H_

#include "frame_common.h"

int process_upload_file_req(transfer_file_req_t *file_info, uint8_t *last_packet);
uint8_t check_upgrade_program_interigy(uint8_t *md5, uint8_t file_type);
uint8_t process_upgrade_program(uint8_t file_type);
#endif
