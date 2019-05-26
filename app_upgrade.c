#include <string.h>

#include "log.h"
#include "type_alias.h"

#include "app_upgrade.h"
#include "upgrade_from_sd_card.h"
#include "upgrade_from_flash.h"

uint16_t g_last_file_pkt_index = 0;


typedef struct kl_upgrade_func {
    void (*init)(uint8_t type);
    uint8_t (*append_file_data)(uint8_t *file_data, uint16_t len, uint8_t last_pkt_flag);
    uint8_t (*check_interigy)(uint8_t type, uint8_t *recv_md5sum);
    uint8_t (*load_app)(uint8_t type);
} kl_upgrade_func_t;


static kl_upgrade_func_t g_flash_upgrade = {
                .init = flash_upload_file_handler_init,
                .append_file_data = flash_buf_append_file_data,
                .check_interigy = flash_integrity_check,
                .load_app = flash_set_app_to_run,
};

static kl_upgrade_func_t g_sd_card_upgrade = {
                .init = sd_card_upload_file_handler_init,
                .append_file_data = sd_card_sppend_file_data,
                .check_interigy = sd_card_integrity_check,
                .load_app = sd_card_set_app_to_run,
};

static kl_upgrade_func_t *g_upgrade_func = &g_flash_upgrade;


uint8_t is_last_packet(uint16_t pkt_count, uint16_t pkt_index)
{
    if (pkt_count == pkt_index + 1) {
        return SD_TRUE;
    }
    else {
        return SD_FALSE;
    }
}

void upload_file_handler_info_cleanup(uint8_t file_type)
{
    if (ARM == file_type || FPGA_CE10 == file_type) {
        g_upgrade_func = &g_flash_upgrade;
    }
    else {//FPGA_CE30
        g_upgrade_func = &g_sd_card_upgrade;
    }

    g_last_file_pkt_index = 0;
    g_upgrade_func->init(file_type);
}

int file_pkt_info_check(uint8_t file_type, uint16_t file_index)
{
    if ((FPGA_CE10 != file_type) && (ARM != file_type) && (FPGA_CE30 != file_type)) {
        LOG_WARN("Invalid file type:%d", file_type);
        return FILECHAOSERR;
    }

    if (g_last_file_pkt_index == file_index && 0 != file_index) {
        //avoid to write the retransmission data
        LOG_DEBUG("Received duplicate upload file packet with index:%d, ignore it and send normal ack.", file_index);
        return DEVICEOK;
    }

    if (file_index != (g_last_file_pkt_index + 1) && 0 != file_index) {
        LOG_WARN("new file_index:%d is not the next file index:%d, reset all.", file_index, g_last_file_pkt_index + 1);
        upload_file_handler_info_cleanup(file_type);
        return FILECHAOSERR;
    }

    return DEVICEOK;

}

int process_upload_file_req(transfer_file_req_t *file_info, uint8_t *last_packet)
{
    int ret = 0;

    if (file_info->file_index == 0) {
        LOG_INFO("Received first upload file packet, cleanup the scene.");
        upload_file_handler_info_cleanup(file_info->file_type);
    }

    ret = file_pkt_info_check(file_info->file_type, file_info->file_index);
    if (ret != DEVICEOK) {
        return ret;
    }

    *last_packet = is_last_packet(file_info->file_count, file_info->file_index);

    ret = g_upgrade_func->append_file_data((uint8_t *)file_info->buff, file_info->data_len, *last_packet);
    if (DEVICEOK == ret) {
        g_last_file_pkt_index = file_info->file_index;
    }

    return ret;
}

uint8_t check_upgrade_program_interigy(uint8_t *md5, uint8_t file_type)
{
    uint8_t rece_md5[33] = { 0 };
    uint8_t errcode = DEVICEOK;
    memcpy(rece_md5, md5, 32);
    errcode = g_upgrade_func->check_interigy(file_type, md5);
    if (DEVICEOK == errcode) {
        LOG_INFO("Received md5sum as: %s, match, try to upgrade.!", rece_md5);
        //FIXME: if file_type == FPGA, how to config image
    }
    else {
        LOG_WARN("Received md5sum as: %s, mismatch, give up upgrade!", rece_md5);
    }
    return errcode;
}

uint8_t process_upgrade_program(uint8_t file_type)
{
	uint8_t errcode = DEVICEOK;
	if (SD_TRUE != g_upgrade_func->load_app(file_type))
		errcode = FPGAUPERR;
    return errcode;
}

