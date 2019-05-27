/*
 * dev_cfg.h
 *
 *  Created on: Mar 21, 2019
 *      Author: chenzy
 */

#ifndef SRC_DEV_CFG_H_
#define SRC_DEV_CFG_H_

#include <stdint.h>
#include "type_alias.h"

#define ID_LEN 17

#define FLASH_PAGE_SIZE    ((uint16_t)0x800)

typedef enum  {
    PHASE_A = 0,
    PHASE_B,
    PHASE_C
}phase_t ;

typedef enum  {
    CUR = 0,
    VOL,
    TEMP,
    HFCT,
    SC_ALARM,
    ALARM
}sensor_t ;

typedef struct _net_info {
    uint8_t ip[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
} net_info_t;

typedef struct _server_info {
    uint8_t ip[4];
    uint16_t port;
    uint8_t domain[64];   //useless
} server_info_t;

typedef struct _product_info {
    uint8_t name[50];//装置名称
    uint8_t type[10];//装置型号
    float version;   //装置基本信息版本号
    uint8_t manufacture[50];//生产厂家
    uint32_t date;   //生产日期
    uint8_t id[20];  //出场编号
} product_info_t;

typedef struct _mx_sensor_info {
    uint16_t exist_flag;
    phase_t phase;
    uint8_t cmd_id[ID_LEN];
    uint8_t component_id[ID_LEN];
    uint16_t interval;  //minutes
    float threshold;
    uint16_t freq;
    float max;
    float min;
    float realtime;
    uint32_t timestamp;
} mx_sensor_info_t;

typedef struct _st_sensor_info {
    uint8_t cmd_id[ID_LEN];
    uint8_t component_id[ID_LEN];
    uint16_t interval;
    float threshold;
    uint16_t defense;//防区屏蔽
    uint16_t status; //现场防区状态
    uint32_t timestamp;
} st_sensor_info_t;//电缆防盗报警监测数据报 (箱门报警 回流缆防盗报警)

typedef struct _sc_sensor_info {
    uint16_t exist_flag;
    phase_t phase;
    uint8_t cmd_id[ID_LEN];
    uint8_t component_id[ID_LEN];
    uint16_t interval;
    float threshold;
    uint16_t current;   //defense or short_circuit current
    uint16_t alarm;     //status or short_circuit alarm
    uint32_t timestamp;
} sc_sensor_info_t;//短路故障判断监测数据报?缺少电流方向

typedef struct _history_count {
    uint32_t total_num;
    uint32_t cur_index;
} history_count_t;

typedef struct _dev_hisroty_count {
    history_count_t cur[3];
    history_count_t vol[3];
    history_count_t temp[3];
    history_count_t hfct[3];
    history_count_t sc_alarm[3];
    history_count_t alarm;
} dev_hisroty_count_t;

typedef struct _dev_info {
    uint32_t flag;          //cfg exist flag

    float current_runtime;  //run time since startup, minutes
    float total_runtime;    //run time since installed, minutes

    product_info_t product;

    net_info_t self;        //useless, read from 4G if user need these info
    server_info_t server;

    mx_sensor_info_t cg_current[3];
    mx_sensor_info_t cg_voltage[3];
    mx_sensor_info_t cg_hfct[3];
    mx_sensor_info_t cg_temp[3];

    sc_sensor_info_t sc_alarm[3];
    st_sensor_info_t alarm;

    dev_hisroty_count_t history;
} dev_info_t;

void test_inner_flash(void);
void init_dev_cfg(void);
void set_default_dev(void);
void show_sensor_info(mx_sensor_info_t* info);


//update saved device information in flash, called only if you need persistent storage
void update_saved_dev_cfg(void);
void load_saved_dev_cfg(void);

//runtime info operations
void update_runtime_info(float add_interval);
float get_current_runtime(void);
float get_total_runtime(void);

//product info operations
void update_dev_product_info(product_info_t* product);
product_info_t* get_dev_product_info(void);
void show_dev_product_info(void);

//self info
net_info_t* get_dev_net_info(void);

//server info operations
void update_dev_server_info(server_info_t *server);
server_info_t* get_dev_server_info(void);
void show_dev_server_info(void);

void update_dev_mx_info_by_type_and_phase(sensor_t type, phase_t phase, mx_sensor_info_t *tmp);

//current info operations
void update_dev_cur_info(phase_t phase, mx_sensor_info_t *tmp);
mx_sensor_info_t* get_dev_cur_info_by_phase(phase_t phase);
boolean get_dev_cur_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info);             //SD_TRUE: success, SD_FLASE: failure
boolean get_dev_cur_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info); //SD_TRUE: success, SD_FLASE: failure

//voltage info operations
void update_dev_vol_info(phase_t phase, mx_sensor_info_t *tmp);
mx_sensor_info_t* get_dev_vol_info_by_phase(phase_t phase);
boolean get_dev_vol_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info);
boolean get_dev_vol_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info);

//temperature info operations
void update_dev_temp_info(phase_t phase, mx_sensor_info_t* tmp);
mx_sensor_info_t* get_dev_temp_info_by_phase(phase_t phase);
boolean get_dev_temp_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info);
boolean get_dev_temp_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info);

//hfct info operations
void update_dev_hfct_info(phase_t phase, mx_sensor_info_t *tmp);
mx_sensor_info_t* get_dev_hfct_info_by_phase(phase_t phase);
boolean get_dev_hfct_info_by_cmd_id(uint8_t *cmd_id, mx_sensor_info_t *info);
boolean get_dev_hfct_info_by_component_id(uint8_t *component_id, mx_sensor_info_t *info);

//sc(Short-Circuit) info operations
void update_dev_sc_alarm_info(phase_t phase, sc_sensor_info_t* tmp);
sc_sensor_info_t* get_dev_sc_alarm_info_by_phase(phase_t phase);
boolean get_dev_sc_alarm_info_by_cmd_id(uint8_t *cmd_id, sc_sensor_info_t *info);
boolean get_dev_sc_alarm_info_by_component_id(uint8_t *component_id, sc_sensor_info_t *info);
void show_sc_alarm_info(phase_t phase);

//alarm info operations
void update_dev_alarm_info(st_sensor_info_t *alarm);
st_sensor_info_t* get_dev_alarm_info(void);
void show_alarm_info(void);

//device history info operations
void update_dev_history_count(dev_hisroty_count_t* history);
dev_hisroty_count_t* get_dev_history_count(void);


#endif /* SRC_DEV_CFG_H_ */
