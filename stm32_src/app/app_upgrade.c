#include <string.h>

#include "log.h"
#include "dev_cfg.h"

#include "app_upgrade.h"
//#include "upgrade_from_sd_card.h"
#include "upgrade_from_flash.h"
#include "periph/rtt.h"
#include "bitfield.h"
#define ENABLE_DEBUG (1)
#include "debug.h"

#define ARM 1
#define FPGA 0
uint16_t g_last_file_pkt_index = 0;

recv_pack_t recv_pack;

typedef struct kl_upgrade_func {
    void (*init)(uint8_t type);
    uint8_t (*append_file_data)(uint8_t *file_data, uint16_t len, uint32_t file_index, uint8_t last_pkt_flag);
    uint8_t (*check_interigy)(uint8_t type, uint8_t *recv_md5sum);
    void (*load_app)(uint8_t type);
} kl_upgrade_func_t;

static kl_upgrade_func_t g_flash_upgrade = {
                .init = flash_upload_file_handler_init,
                .append_file_data = hardcode_addr_flash_buf_append_file_data,
                .check_interigy = flash_integrity_check,
                .load_app = flash_set_app_to_run,
};

//static kl_upgrade_func_t g_sd_card_upgrade = {
//                .init = sd_card_upload_file_handler_init,
//                .append_file_data = sd_card_sppend_file_data,
//                .check_interigy = sd_card_integrity_check,
//                .load_app = sd_card_set_app_to_run,
//};

static kl_upgrade_func_t *g_upgrade_func = &g_flash_upgrade;
recv_pack_t get_recv_pack_info(void)
{
	return recv_pack;
}

void add_recv_pkt_info(uint32_t file_count, uint32_t file_index)
{
    recv_pack.file_count = file_count;
    bf_set(recv_pack.bit_filed, file_index);
}

void clear_recv_file_info(void)
{
    memset((void*)&recv_pack, 0x0, sizeof(recv_pack_t));
}

//result: SD_FALSE: missed some file content, SD_TRUE: all file content reveived

uint8_t check_file_info_integrity(uint32_t count)
{
    uint32_t i = 0;
    uint8_t result = SD_TRUE;
    recv_pack.file_count = count;

    for (i = 0; i < recv_pack.file_count; i++) {
        if (0 == bf_isset(recv_pack.bit_filed ,i)) {
            result = SD_FALSE;
            break;
        }
    }
    if (result == SD_TRUE) {
        g_upgrade_func->check_interigy(ARM, NULL);
    }

    return result;
}

uint8_t is_last_packet(uint16_t pkt_count, uint16_t pkt_index)
{
    if (pkt_count == pkt_index ) {
        return SD_TRUE;
    }
    else {
        return SD_FALSE;
    }
}

void upload_file_handler_info_cleanup(uint8_t file_type)
{
    if (ARM == file_type || FPGA == file_type) {
        g_upgrade_func = &g_flash_upgrade;
    }
//    else {
//        g_upgrade_func = &g_sd_card_upgrade;
//    }

    g_last_file_pkt_index = 0;
    g_upgrade_func->init(file_type);
}

int file_pkt_info_check(uint8_t file_type, uint16_t file_index)
{
    if ((FPGA != file_type) && (ARM != file_type) && (2 != file_type) && (3 != file_type)) {
        DEBUG("Invalid file type:%d\r\n", file_type);
        return 8;
    }
//    upload_file_handler_info_cleanup(file_type);
    (void)file_index;
    return 0;

    if (g_last_file_pkt_index == file_index && 0 != file_index) {
        //avoid to write the retransmission data
        LOG_DEBUG("Received duplicate upload file packet with index:%d, ignore it and send normal ack.", file_index);
        return 0;
    }

    if (file_index != (g_last_file_pkt_index + 1) && 0 != file_index) {
        DEBUG("new file_index:%d is not the next file index:%d, reset all.", file_index, g_last_file_pkt_index + 1);
        upload_file_handler_info_cleanup(file_type);
        return 8;
    }

    return 0;

}

int process_upload_file_req(transfer_file_req_t *file_info, uint32_t file_index, uint8_t *last_packet)
{
    int ret = 0;

    if (file_info->file_index == 0) {
        LOG_INFO("Received first upload file packet, cleanup the scene.");
        upload_file_handler_info_cleanup(file_info->file_type);
    }

    ret = file_pkt_info_check(file_info->file_type, file_info->file_index);
    if (ret != 0) {
        return ret;
    }

    *last_packet = is_last_packet(file_info->file_count, file_info->file_index);

    ret = g_upgrade_func->append_file_data((uint8_t *)file_info->buff, file_info->data_len, file_index, *last_packet);
    if (0 == ret) {
        g_last_file_pkt_index = file_info->file_index;
    }

    return ret;
}

void process_upgrade_programe_req(uint8_t file_type)
{
        g_upgrade_func->load_app(file_type);
}

#define HALF_HOUR (60 * 30)
#define ARM_FILE_TRANSFER_TIMEOUT HALF_HOUR
static uint32_t g_arm_file_transfer_flag = SD_FALSE;
static uint32_t g_start_transfer_timestamp = 0;
void set_arm_file_transfer_flag(uint32_t flag)
{
	g_arm_file_transfer_flag = flag;
	if (SD_TRUE == g_arm_file_transfer_flag) {
		g_start_transfer_timestamp = rtt_get_counter();
	}
}

uint32_t get_upgrade_arm_flag(void)
{
	return g_arm_file_transfer_flag;
}

uint32_t is_arm_file_transfer_timeout(void)
{
	return rtt_get_counter() - g_start_transfer_timestamp > ARM_FILE_TRANSFER_TIMEOUT ? SD_TRUE : SD_FALSE;
}

