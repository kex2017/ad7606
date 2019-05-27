/*
 * dev_cfg.c
 *
 *  Created on: Mar 21, 2019
 *      Author: chenzy
 */

#include <stdio.h>
#include "string.h"

#include "log.h"
#include "periph/flashpage.h"
#include "periph/rtt.h"

#include "dev_cfg.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

//#define DEFAULT_SERVER_PORT 6891
#define DEFAULT_SERVER_PORT 6892

#define ENV_CFGBASE_ADDR           0x08078800  //30k
#define CPU_FLASH_BASE_ADDR           0x08000000  //512k
#define CFG_FLAG_OFF     0x12121200
#define CFG_FLAG_ON      0x21212100

#define DEV_VERSION (3.0)

typedef union {
   uint8_t env_buf[FLASH_PAGE_SIZE];
   dev_info_t dev;
} env_dev_t;

static dev_info_t *g_dev_info = NULL;
static env_dev_t g_env_dev;

/*************************************************************************
 * operations for runtime info
*************************************************************************/
void reset_runtime_info(void)
{
    g_dev_info->current_runtime = 0;
    g_dev_info->total_runtime = 0;
}

void update_runtime_info(float add_interval)
{
    g_dev_info->current_runtime += add_interval;
    g_dev_info->total_runtime += add_interval;

    update_saved_dev_cfg();
}

float get_current_runtime(void)
{
    return g_dev_info->current_runtime;
}

float get_total_runtime(void)
{
    return g_dev_info->total_runtime;
}

void show_strings(uint8_t *string, uint16_t len)
{
    int i = 0;

    printf("\t");
    for (i = 0; (i < len) && (*string != '\0'); i++) {
        printf("%c", *string++);
    }
    printf("\r\n");
}

/*************************************************************************
 * operations for product info
*************************************************************************/
void set_dev_default_product_info(void)
{
    product_info_t product;

    memset((void*)&product, 0x0, sizeof(product_info_t));
    snprintf((char*)product.name, sizeof(product.name), "%s", "SWTR");
    snprintf((char*)product.type, sizeof(product.type), "%s", "KLJC-28");
    product.version = DEV_VERSION;
    snprintf((char*)product.manufacture, sizeof(product.manufacture), "%s", "KLJC");
    product.date = 1553218493; //2019/3/22 9:34:53
    snprintf((char*)product.id, sizeof(product.id), "%s", "SN28190001");

    g_dev_info->product = product;
}

void update_dev_product_info(product_info_t* product)
{
    g_dev_info->product = *product;

    update_saved_dev_cfg();
}

product_info_t* get_dev_product_info(void)
{
    return &g_dev_info->product;
}

void show_dev_product_info(void)
{
	LOG_INFO("[DEV_CFG]:\r\n");
	LOG_INFO("name:\r\n");
    show_strings(g_dev_info->product.name, 50);
    LOG_INFO("type:\r\n");
    show_strings(g_dev_info->product.type, 10);
    LOG_INFO("version: %d\r\n", (int)g_dev_info->product.version);
    LOG_INFO("manufacture:\r\n");
    show_strings(g_dev_info->product.manufacture, 50);
    LOG_INFO("date: %ld\r\n", g_dev_info->product.date);
    LOG_INFO("id:\r\n");
    show_strings(g_dev_info->product.id, 20);
}


void set_dev_mock_self_info(void)
{
    net_info_t self;

    memset((void*)&self, 0x0, sizeof(net_info_t));

    self.ip[0] = 127;
    self.ip[1] = 0;
    self.ip[2] = 0;
    self.ip[3] = 1;

    self.netmask[0] = 255;
    self.netmask[1] = 255;
    self.netmask[2] = 255;
    self.netmask[3] = 0;

    self.dns[0] = 8;
    self.dns[1] = 8;
    self.dns[2] = 8;
    self.dns[3] = 8;

    g_dev_info->self = self;
}


/*************************************************************************
 * operations for server info
*************************************************************************/
void set_dev_default_server_info(void)
{
	server_info_t server;

	memset((void*) &server, 0x0, sizeof(server_info_t));
	server.port = DEFAULT_SERVER_PORT;
    server.ip[0] = 122;
    server.ip[1] = 224;
    server.ip[2] = 250;
    server.ip[3] = 234;
//	server.ip[0] = 211;
//	server.ip[1] = 155;
//	server.ip[2] = 226;
//	server.ip[3] = 190;

	snprintf((char*) server.domain, sizeof(server.domain), "%s", "localhost");
	server.domain[10] = '\0';

	g_dev_info->server = server;
}

net_info_t* get_dev_net_info(void)
{
	return  &g_dev_info->self;
}

void update_dev_server_info(server_info_t* server)
{
    g_dev_info->server = *server;

    update_saved_dev_cfg();
}

server_info_t *get_dev_server_info(void)
{
    return &g_dev_info->server;
}

void show_dev_server_info(void)
{
	LOG_INFO("[DEV_CFG]:");
	LOG_INFO("domain:");
    show_strings(g_dev_info->server.domain, 64);
    LOG_INFO("ip: %d.%d.%d.%d, port:%d", g_dev_info->server.ip[0], g_dev_info->server.ip[1], g_dev_info->server.ip[2], g_dev_info->server.ip[3], g_dev_info->server.port);
}


void show_sensor_info(mx_sensor_info_t* info)
{
	LOG_INFO("[DEV_CFG]:");
	LOG_INFO("exist_flag:%d", info->exist_flag);
	LOG_INFO("cmd_id:");
    show_strings(info->cmd_id, ID_LEN);
    LOG_INFO("component_id");
    show_strings(info->component_id, ID_LEN);
    LOG_INFO("interval: %d, threshold:%d\r\n", info->interval, (int)info->threshold);
    LOG_INFO("timestamp:%ld, max: %ld, min: %ld, realtime: %ld\r\n", info->timestamp, (uint32_t)info->max, (uint32_t)info->min, (uint32_t)info->realtime);
}

/*************************************************************************
 * operations for self net info
*************************************************************************/

/*************************************************************************
 * operations for cgibox current info
*************************************************************************/
void set_dev_default_cur_info(void)
{
    int i = 0;
    mx_sensor_info_t tmp = {0};

    for (i = 0; i < 3; i++) {
        tmp.exist_flag = 1;
        tmp.phase = i;
//        snprintf((char*)tmp.cmd_id, sizeof(tmp.cmd_id), "cgCur00000123456%d", i);
//        snprintf((char*)tmp.component_id, sizeof(tmp.component_id), "cgCur00000123456%d", i);
        tmp.interval = 20;
        tmp.threshold = 100;
        g_dev_info->cg_current[i] = tmp;
    }

    memcpy((void*)g_dev_info->cg_current[0].cmd_id, "HZKLZNJDX00000002", ID_LEN);
    memcpy((void*)g_dev_info->cg_current[1].cmd_id, "HZKLZNJDX00000003", ID_LEN);
    memcpy((void*)g_dev_info->cg_current[2].cmd_id, "HZKLZNJDX00000004", ID_LEN);

    memcpy((void*)g_dev_info->cg_current[0].component_id, "HZKLZNJDX00000002", ID_LEN);
    memcpy((void*)g_dev_info->cg_current[1].component_id, "HZKLZNJDX00000003", ID_LEN);
    memcpy((void*)g_dev_info->cg_current[2].component_id, "HZKLZNJDX00000004", ID_LEN);

}

void update_dev_mx_info_by_type_and_phase(sensor_t type, phase_t phase, mx_sensor_info_t* tmp)
{
    switch (type) {
    case CUR:
        update_dev_cur_info(phase, tmp);
        break;
    case VOL:
        update_dev_vol_info(phase, tmp);
        break;
    case TEMP:
        update_dev_temp_info(phase, tmp);
        break;
    case HFCT:
        update_dev_hfct_info(phase, tmp);
        break;
    default:
        DEBUG("[DEV_CFG]:Invalid mx sensor type.");
        break;
    }
}

void update_dev_cur_info(phase_t phase, mx_sensor_info_t* tmp)
{
    g_dev_info->cg_current[phase] = *tmp;

    update_saved_dev_cfg();
}

mx_sensor_info_t* get_dev_cur_info_by_phase(phase_t phase)
{
    return &g_dev_info->cg_current[phase];
}

boolean get_dev_cur_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)cmd_id, (char*)get_dev_cur_info_by_phase(i)->cmd_id, ID_LEN)) {
            *info = g_dev_info->cg_current[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

boolean get_dev_cur_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)component_id, (char*)get_dev_cur_info_by_phase(i)->component_id, ID_LEN)) {
            *info = g_dev_info->cg_current[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}


/*************************************************************************
 * operations for cgibox voltage info
*************************************************************************/
void set_dev_default_vol_info(void)
{
    int i = 0;
    mx_sensor_info_t tmp = {0};

    for (i = 0; i < 3; i++) {
        tmp.exist_flag = 1;
        tmp.phase = i;
//        snprintf((char*)tmp.cmd_id, sizeof(tmp.cmd_id), "cgVol00000123456%d", i);
//        snprintf((char*)tmp.component_id, sizeof(tmp.component_id), "cgVol00000123456%d", i);
        tmp.interval = 20;
        tmp.threshold = 100;
        g_dev_info->cg_voltage[i] = tmp;
    }

    memcpy((void*)g_dev_info->cg_voltage[0].cmd_id, "HZKLZNJDX00000005", ID_LEN);
    memcpy((void*)g_dev_info->cg_voltage[1].cmd_id, "HZKLZNJDX00000006", ID_LEN);
    memcpy((void*)g_dev_info->cg_voltage[2].cmd_id, "HZKLZNJDX00000007", ID_LEN);

    memcpy((void*)g_dev_info->cg_voltage[0].component_id, "HZKLZNJDX00000005", ID_LEN);
    memcpy((void*)g_dev_info->cg_voltage[1].component_id, "HZKLZNJDX00000006", ID_LEN);
    memcpy((void*)g_dev_info->cg_voltage[2].component_id, "HZKLZNJDX00000007", ID_LEN);
}

void update_dev_vol_info(phase_t phase, mx_sensor_info_t* tmp)
{
    g_dev_info->cg_voltage[phase] = *tmp;

    update_saved_dev_cfg();
}

mx_sensor_info_t* get_dev_vol_info_by_phase(phase_t phase)
{
    return &g_dev_info->cg_voltage[phase];
}

boolean get_dev_vol_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)cmd_id, (char*)get_dev_vol_info_by_phase(i)->cmd_id, ID_LEN)) {
            *info = g_dev_info->cg_voltage[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

boolean get_dev_vol_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)component_id, (char*)get_dev_vol_info_by_phase(i)->component_id, ID_LEN)) {
            *info = g_dev_info->cg_voltage[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

/*************************************************************************
 * operations for cgibox temperature info
*************************************************************************/
void set_dev_default_temp_info(void)
{
    int i = 0;
    mx_sensor_info_t tmp = {0};

    for (i = 0; i < 3; i++) {
        tmp.exist_flag = 1;
        tmp.phase = i;
//        snprintf((char*)tmp.cmd_id, sizeof(tmp.cmd_id), "cgTemp0000123456%d", i);
//        snprintf((char*)tmp.component_id, sizeof(tmp.component_id), "cgTemp0000123456%d", i);
        tmp.interval = 20;
        tmp.threshold = 100;
        g_dev_info->cg_temp[i] = tmp;
    }

    memcpy((void*)g_dev_info->cg_temp[0].cmd_id, "HZKLZNJDX00000008", ID_LEN);
    memcpy((void*)g_dev_info->cg_temp[1].cmd_id, "HZKLZNJDX00000009", ID_LEN);
    memcpy((void*)g_dev_info->cg_temp[2].cmd_id, "HZKLZNJDX00000010", ID_LEN);

    memcpy((void*)g_dev_info->cg_temp[0].component_id, "HZKLZNJDX00000008", ID_LEN);
    memcpy((void*)g_dev_info->cg_temp[1].component_id, "HZKLZNJDX00000009", ID_LEN);
    memcpy((void*)g_dev_info->cg_temp[2].component_id, "HZKLZNJDX0000000a", ID_LEN);
}

void update_dev_temp_info(phase_t phase, mx_sensor_info_t* tmp)
{
    g_dev_info->cg_temp[phase] = *tmp;

    update_saved_dev_cfg();
}

mx_sensor_info_t* get_dev_temp_info_by_phase(phase_t phase)
{
    return &g_dev_info->cg_temp[phase];
}

boolean get_dev_temp_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)cmd_id, (char*)get_dev_vol_info_by_phase(i)->cmd_id, ID_LEN)) {
            *info = g_dev_info->cg_temp[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

boolean get_dev_temp_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)component_id, (char*)get_dev_vol_info_by_phase(i)->component_id, ID_LEN)) {
            *info = g_dev_info->cg_temp[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

/*************************************************************************
 * operations for cgibox hfct info
*************************************************************************/
void set_dev_default_hfct_info(void)
{
    int i = 0;
    mx_sensor_info_t tmp = {0};

    for (i = 0; i < 3; i++) {
        tmp.exist_flag = 1;
        tmp.phase = i;
//        snprintf((char*)tmp.cmd_id, sizeof(tmp.cmd_id), "cgHfct0000123456%d", i);
//        snprintf((char*)tmp.component_id, sizeof(tmp.component_id), "cgHfct0000123456%d", i);
        tmp.interval = 20;
        tmp.threshold = 100;
        g_dev_info->cg_hfct[i] = tmp;
    }

    memcpy((void*)g_dev_info->cg_hfct[0].cmd_id, "HZKLZNJDX00000008", ID_LEN);
    memcpy((void*)g_dev_info->cg_hfct[1].cmd_id, "HZKLZNJDX00000009", ID_LEN);
    memcpy((void*)g_dev_info->cg_hfct[2].cmd_id, "HZKLZNJDX0000000a", ID_LEN);

    memcpy((void*)g_dev_info->cg_hfct[0].component_id, "HZKLZNJDX00000008", ID_LEN);
    memcpy((void*)g_dev_info->cg_hfct[1].component_id, "HZKLZNJDX00000009", ID_LEN);
    memcpy((void*)g_dev_info->cg_hfct[2].component_id, "HZKLZNJDX0000000a", ID_LEN);

}

void update_dev_hfct_info(phase_t phase, mx_sensor_info_t* tmp)
{
    g_dev_info->cg_hfct[phase] = *tmp;

    update_saved_dev_cfg();
}

mx_sensor_info_t* get_dev_hfct_info_by_phase(phase_t phase)
{
    return &g_dev_info->cg_hfct[phase];
}

boolean get_dev_hfct_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)cmd_id, (char*)get_dev_hfct_info_by_phase(i)->cmd_id, ID_LEN)) {
            *info = g_dev_info->cg_hfct[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

boolean get_dev_hfct_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info)
{
    int i = 0;
    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)component_id, (char*)get_dev_vol_info_by_phase(i)->component_id, ID_LEN)) {
            *info = g_dev_info->cg_hfct[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

/*************************************************************************
 * operations for cgibox short-circuit alarm info
*************************************************************************/
void set_dev_default_sc_alarm_info(void)
{
    int i = 0;
    sc_sensor_info_t tmp = {0};

    for (i = 0; i < 3; i++) {
        tmp.exist_flag = 1;
        tmp.phase = i;
//        snprintf((char*)tmp.cmd_id, sizeof(tmp.cmd_id), "cgScAlarm0123456%d", i);
//        snprintf((char*)tmp.component_id, sizeof(tmp.component_id), "cgScAlarm0123456%d", i);
        tmp.interval = 20;
        tmp.threshold = 100;
        g_dev_info->sc_alarm[i] = tmp;
    }

    memcpy((void*)g_dev_info->sc_alarm[0].cmd_id, "HZKLZNJDX0000000b", ID_LEN);
    memcpy((void*)g_dev_info->sc_alarm[1].cmd_id, "HZKLZNJDX0000000c", ID_LEN);
    memcpy((void*)g_dev_info->sc_alarm[2].cmd_id, "HZKLZNJDX0000000d", ID_LEN);

    memcpy((void*)g_dev_info->sc_alarm[0].component_id, "HZKLZNJDX0000000b", ID_LEN);
    memcpy((void*)g_dev_info->sc_alarm[1].component_id, "HZKLZNJDX0000000c", ID_LEN);
    memcpy((void*)g_dev_info->sc_alarm[2].component_id, "HZKLZNJDX0000000d", ID_LEN);


}

void update_dev_sc_alarm_info(phase_t phase, sc_sensor_info_t* tmp)
{
    g_dev_info->sc_alarm[phase] = *tmp;

    update_saved_dev_cfg();
}

sc_sensor_info_t* get_dev_sc_alarm_info_by_phase(phase_t phase)
{
    return &g_dev_info->sc_alarm[phase];
}

boolean get_dev_sc_alarm_info_by_cmd_id(uint8_t *cmd_id, sc_sensor_info_t *info)
{
    int i = 0;

    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)cmd_id, (char*)get_dev_sc_alarm_info_by_phase(i)->cmd_id, ID_LEN)) {
            *info = g_dev_info->sc_alarm[i];
            return SD_TRUE;
        }
    }
    return SD_FALSE;
}

boolean get_dev_sc_alarm_info_by_component_id(uint8_t *component_id, sc_sensor_info_t *info)
{
    int i = 0;

    for (i = 0; i < 3; i++) {
        if (!strncmp((char*)component_id, (char*)get_dev_sc_alarm_info_by_phase(i)->component_id, ID_LEN)) {
            *info = g_dev_info->sc_alarm[i];
            return SD_TRUE;
        }
    }

    return SD_FALSE;
}

void show_sc_alarm_info(phase_t phase)
{
	LOG_INFO("[DEV_CFG]:");
	LOG_INFO("cmd_id:");
    show_strings(g_dev_info->sc_alarm[phase].cmd_id, ID_LEN);
    LOG_INFO("component_id");
    show_strings(g_dev_info->sc_alarm[phase].component_id, ID_LEN);
    LOG_INFO("interval:%d, threshold:%d, current:0x%04x, alarm:0x%04x, timestamp:%ld",
             g_dev_info->sc_alarm[phase].interval, (int)g_dev_info->sc_alarm[phase].threshold,
             g_dev_info->sc_alarm[phase].current, g_dev_info->sc_alarm[phase].alarm,
             g_dev_info->sc_alarm[phase].timestamp);
}

/*************************************************************************
 * operations for cgibox alarm info
*************************************************************************/
void set_dev_default_alarm_info(void)
{
    st_sensor_info_t alarm;

    memcpy((void*)g_dev_info->alarm.cmd_id, "HZKLZNJDX0000000e", ID_LEN);
    memcpy((void*)g_dev_info->alarm.component_id, "HZKLZNJDX0000000e", ID_LEN);
//    snprintf((char*)alarm.cmd_id, sizeof(alarm.cmd_id), "cgAlarm0001234567");
//    snprintf((char*)alarm.component_id, sizeof(alarm.component_id), "cgAlarm0001234567");
    alarm.interval = 20;
    alarm.threshold = 100;
    alarm.defense = 0x1F;
    alarm.status = 0;

    g_dev_info->alarm = alarm;
}

void update_dev_alarm_info(st_sensor_info_t* alarm)
{
    g_dev_info->alarm = *alarm;

    update_saved_dev_cfg();
}

st_sensor_info_t* get_dev_alarm_info(void)
{
    return &g_dev_info->alarm;
}

void show_alarm_info(void)
{
	LOG_INFO("cmd_id:");
    show_strings(g_dev_info->alarm.cmd_id, ID_LEN);
    LOG_INFO("component_id");
    show_strings(g_dev_info->alarm.component_id, ID_LEN);
    LOG_INFO("interval:%d, threshold:%d, defense:0x%04x, status:0x%04x, timestamp:%ld\r\n", g_dev_info->alarm.interval, (int)g_dev_info->alarm.threshold, g_dev_info->alarm.defense, g_dev_info->alarm.status, g_dev_info->alarm.timestamp);
}

/*************************************************************************
 * operations for device history count info
*************************************************************************/
dev_hisroty_count_t* get_dev_history_count(void)
{
    return &g_dev_info->history;
}

void update_dev_history_count(dev_hisroty_count_t* history)
{
    g_dev_info->history = *history;

    update_saved_dev_cfg();
}

void reset_dev_history_data_info(void)
{
    memset((void*)&g_dev_info->history, 0x0, sizeof(dev_hisroty_count_t));
}


void set_default_dev(void)
{
    memset(g_dev_info, 0x0, FLASH_PAGE_SIZE);

    update_runtime_info(0);
    set_dev_default_product_info();
    set_dev_mock_self_info();

    set_dev_default_server_info();

    set_dev_default_cur_info();
    set_dev_default_vol_info();
    set_dev_default_temp_info();
    set_dev_default_hfct_info();

    set_dev_default_sc_alarm_info();
    set_dev_default_alarm_info();

    reset_dev_history_data_info();
    LOG_INFO("Set device info as default.");
}

void update_saved_dev_cfg(void)
{
    uint32_t page = 0;

    g_env_dev.dev.flag = CFG_FLAG_ON;
    page = (ENV_CFGBASE_ADDR - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    flashpage_write(page, (void*)g_env_dev.env_buf);
}

void load_saved_dev_cfg(void)
{
    memcpy((void*)g_env_dev.env_buf, (void*)ENV_CFGBASE_ADDR, FLASH_PAGE_SIZE);
}

void init_dev_cfg(void)
{
    memset(g_env_dev.env_buf, 0x0, FLASH_PAGE_SIZE);
    g_dev_info = &g_env_dev.dev;

    load_saved_dev_cfg();
    LOG_INFO("Got cfg flag as 0x%08x", (int)g_dev_info->flag);
    if (CFG_FLAG_ON != g_dev_info->flag) {
        set_default_dev();
        update_saved_dev_cfg();
    }
    else {
    	LOG_INFO("Load saved device info.");
    }
    g_dev_info->current_runtime = 0;
    g_dev_info->product.version = DEV_VERSION;
}

static uint8_t temp[FLASH_PAGE_SIZE];
void test_inner_flash(void)
{
    uint32_t page = 0;

    uint8_t data[4] = {0};
    uint32_t time = 0;

    page = (ENV_CFGBASE_ADDR - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE+1;

    time = rtt_get_counter();
    temp[0]= (time & 0xFF000000) >> 24;
    temp[1]= (time & 0x00FF0000) >> 16;
    temp[2]= (time & 0x0000FF00) >> 8;
    temp[3]= (time & 0x000000FF);
    flashpage_write(page, temp);

    memcpy(data, (uint32_t *)(ENV_CFGBASE_ADDR + FLASH_PAGE_SIZE), 4);
    LOG_INFO("Got time as 0x%08x, data as: 0x%02x, 0x%02x, 0x%02x, 0x%02x", (int)time, (int)data[0], (int)data[1], (int)data[2], (int)data[3]);

    load_saved_dev_cfg();
    LOG_INFO("Got cfg flag as 0x%08x", (int)g_dev_info->flag);

}







