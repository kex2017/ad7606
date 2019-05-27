/*
 * sc_cfg.c
 *
 *  Created on: Mar 22, 2019
 *      Author: chenzy
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "periph/rtt.h"

#include "log.h"

#include "sc_cfg.h"
#include "dev_cfg.h"
#include "history_data.h"
#define ENABLE_DEBUG (1)
#include "debug.h"

void cfg_usage_help(void)
{
    puts("\nNAME:");
    puts("  cfg -- set device configurations");
    puts("usage: cfg [-h | --help] <command> [<args>]");
    puts("\nCOMMANDS");
    puts("  read    read device cfg");
    puts("  write   write device cfg");
    puts("  reset   reset device default cfg");
    puts("  saveenv write device cfg to flash");
    puts("  loadenv read device cfg from flash");
    puts("  history test history data read and write");
    puts("  time    set device timestamp");
    puts("  test    test flash");
}

void read_cfg_uasge(void)
{
    puts("\nusage: cfg read <option>");
    puts("  -t, --type <>       cfg type: cur/vol/temp/hfct/alarm/product/self/server");
    puts("  -p, --phase <>      cfg phase: a/b/c");
//    puts("  -k, --key <>        cfg name:");
    puts("  -h, --help          show help message");
}

void write_cfg_uasge(void)
{
    puts("\nusage: cfg write <option>");
    puts("  -t, --type <>       cfg type: cur/vol/temp/hfct/alarm/runtime/product/self/server");
    puts("  -p, --phase <>      cfg phase: a/b/c");
    puts("  -k, --key <>        cfg name:cmd_id, component_id, interval, threshold, id(product_id)...");
    puts("  -v, --value <>      cfg value");
    puts("  -h, --help          show help message");
}

void history_cfg_uasge(void)
{
    puts("\nusage: cfg history <option>");
    puts("  -m, --mock          history mock: mock 3 data");
    puts("  -t, --type <>       history type: cur/vol/temp/hfct/alarm");
    puts("  -p, --phase <>      history phase: a/b/c");
    puts("  -i, --index <>      history data index");
    puts("  -h, --help          show help message");
}


phase_t get_phase_by_char(char c)
{
    if (c == 'a' || c == 'A')
        return PHASE_A;
    if (c == 'b' || c == 'B')
        return PHASE_B;
    if (c == 'c' || c == 'C')
        return PHASE_C;

    return PHASE_A;
}

sensor_t get_type_by_string(char *string)
{
    if (0 == strncmp(string, "cur", 3))
        return CUR;
    if (0 == strncmp(string, "vol", 3))
        return VOL;
    if (0 == strncmp(string, "temp", 4))
        return TEMP;
    if (0 == strncmp(string, "hfct", 4))
        return HFCT;
    if (0 == strncmp(string, "alarm", 5))
        return ALARM;

    return CUR;
}

char* get_type_desc(sensor_t type)
{
    char *tmp = NULL;
    switch (type) {
    case CUR:
        tmp = "CUR";
        break;
    case VOL:
        tmp = "VOL";
        break;
    case TEMP:
        tmp = "TEMP";
        break;
    case HFCT:
        tmp = "HFCT";
        break;
    case ALARM:
        tmp = "ALARM";
        break;
    case SC_ALARM:
        tmp = "SC_ALARM";
        break;
    }

    return tmp;
}

char* get_phase_desc(phase_t p)
{
    char *tmp = NULL;
    switch (p) {
    case PHASE_A:
        tmp = "phase A";
        break;
    case PHASE_B:
        tmp = "phase B";
        break;
    case PHASE_C:
        tmp = "phase C";
        break;
    }

    return tmp;
}

mx_sensor_info_t* get_mx_sensor_info(char *type, phase_t p)
{
    mx_sensor_info_t* info = { 0 };

    if (strncmp(type, "cur", 3) == 0) {
        info = get_dev_cur_info_by_phase(p);
    }
    else if (strncmp(type, "vol", 3) == 0) {
        info = get_dev_vol_info_by_phase(p);
    }
    else if (strncmp(type, "temp", 4) == 0) {
        info = get_dev_temp_info_by_phase(p);
    }
    else if (strncmp(type, "hfct", 4) == 0) {
        info = get_dev_hfct_info_by_phase(p);
    }

    return info;
}

void set_mx_sensor_info(char *type, phase_t p, mx_sensor_info_t* info)
{
    if (strncmp(type, "cur", 3) == 0) {
        update_dev_cur_info(p, info);
    }
    else if (strncmp(type, "vol", 3) == 0) {
        update_dev_vol_info(p, info);
    }
    else if (strncmp(type, "temp", 4) == 0) {
        update_dev_temp_info(p, info);
    }
    else if (strncmp(type, "hfct", 4) == 0) {
        update_dev_hfct_info(p, info);
    }
}

void show_history_data_count(void)
{
	history_count_t history;
	history = get_history_count_info(CUR, 0);
	LOG_INFO("History data count: %ld", history.total_num);
}

int read_cfg_command(int argc, char **argv)
{
    int opt;
    char type[20] = {0};
    phase_t p = PHASE_A;
    mx_sensor_info_t* info;

    static const struct option long_opts[] = {
            { "help", no_argument, NULL, 'h' },
            { "type", required_argument, NULL, 't' },
            { "phase", required_argument, NULL, 'p' },
            { NULL, 0, NULL, 0 },
    };

    if (argc < 2) {
        read_cfg_uasge();
        return 1;
    }

    while ((opt = getopt_long(argc, argv, "ht:p:r:", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            read_cfg_uasge();
            return 0;
        case 't':
            strcpy(type, optarg);
            break;
        case 'p':
            p = get_phase_by_char(*optarg);
            break;
        default:
            DEBUG("Unsupported read option.");
            return 0;
            break;
        }
    }

    if (strncmp(type, "runtime", 7) == 0) {
    	DEBUG("Show runtime info: total:%ld, current:%ld", (uint32_t)get_total_runtime(), (uint32_t)get_current_runtime());
    }
    else if (strncmp(type, "alarm", 5) == 0) {
    	DEBUG("Show alarm info:");
        show_alarm_info();
    }
    else if (strncmp(type, "sc", 2) == 0) {
    	DEBUG("Show sc alarm info:");
        show_sc_alarm_info(p);
    }
    else if (strncmp(type, "product", 7) == 0) {
    	DEBUG("Show product info:");
        show_dev_product_info();
    }
    else if (strncmp(type, "self", 4) == 0) {
    	DEBUG("Show self info:");
        //FIXME: show self
    }
    else if (strncmp(type, "server", 6) == 0) {
    	DEBUG("Show server info:");
        show_dev_server_info();
    }
    else if ((strncmp(type, "cur", 3) == 0) || (strncmp(type, "vol", 3) == 0) || (strncmp(type, "temp", 4) == 0) || (strncmp(type, "hfct", 4) == 0)) {
        info = get_mx_sensor_info(type, p);
        DEBUG("Show sensor %s info:", get_phase_desc(p));
        show_sensor_info(info);
    }

    optind = 1;

    return 0;
}

void update_mx_cfg_by_type_and_phase(char *type, char *key, phase_t p, float f_value, char *s_value)
{
    mx_sensor_info_t* info = NULL;
    info = get_mx_sensor_info(type, p);
    if (strncmp(key, "flag", 16) == 0) {
        info->exist_flag = !!f_value;
    }
    if (strncmp(key, "cmd_id", 6) == 0) {
        snprintf((char*)info->cmd_id, sizeof(info->cmd_id), "%s", s_value);
    }
    if (strncmp(key, "component_id", 16) == 0) {
        snprintf((char*)info->component_id, sizeof(info->component_id), "%s", s_value);
    }
    if (strncmp(key, "interval", 16) == 0) {
        info->interval = (uint16_t)f_value;
    }
    if (strncmp(key, "threshold", 16) == 0) {
        info->threshold = (uint16_t)f_value;
    }

    set_mx_sensor_info(type, p, info);
    DEBUG("Show sensor %s info:", get_phase_desc(p));
    show_sensor_info(info);
}

float get_value_if_need(char *key, char *s_value)
{
    float f_value = 0;
    if ((0 == strncmp(key, "cmd_id", 6)) || (0 == strncmp(key, "component_id", 16)) || (0 == strncmp(key, "id", 2))
                    || (0 == strncmp(key, "ip", 6))) {
        //do nothing
    }
    else if ((0 == strncmp(key, "version", 6)) || (0 == strncmp(key, "flag", 4)) || (0 == strncmp(key, "interval", 8))
                    || (0 == strncmp(key, "threshold", 8)) || (0 == strncmp(key, "port", 6))
                    || (0 == strncmp(key, "date", 4))) {
        f_value = (float)atof(s_value);
        DEBUG("Got f value as:%ld", (uint32_t)f_value);
    }

    return f_value;
}

int write_cfg_command(int argc, char **argv)
{
    int opt;
    char type[20] = {0};
    char key[20] = {0};
    char s_value[20] = {0};
    float f_value = 0;
    phase_t p = PHASE_A;
    st_sensor_info_t* alarm;
    sc_sensor_info_t* sc;
    product_info_t* product = {0};
    server_info_t* server = {0};
    char *ip_section[4] = { 0 };
    char delimiters = '.';
    int i = 0;

    static const struct option long_opts[] = {
            { "help", no_argument, NULL, 'h' },
            { "type", required_argument, NULL, 't' },
            { "phase", required_argument, NULL, 'p' },
            { "key", required_argument, NULL, 'k' },
            { "value", required_argument, NULL, 'v' },
            { NULL, 0, NULL, 0 },
    };

    if (argc < 2) {
        write_cfg_uasge();
        return 1;
    }

    while ((opt = getopt_long(argc, argv, "ht:p:k:v:", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            write_cfg_uasge();
            return 0;
            break;
        case 't':
            strcpy(type, optarg);
            break;
        case 'p':
            p = get_phase_by_char(*optarg);
            break;
        case 'k':
            strcpy(key, optarg);
            break;
        case 'v':
            strcpy(s_value, optarg);
            break;
        default:
        	LOG_INFO("Unsupported read option.");
            return 0;
            break;
        }
    }

    f_value = get_value_if_need(key, s_value);

    if (strncmp(type, "alarm", 5) == 0 && strncmp(key, "interval", 5) == 0) {//alarm only support interval
        alarm = get_dev_alarm_info();
        alarm->interval = (uint16_t)f_value;
        update_dev_alarm_info(alarm);

        LOG_INFO("Show alarm info:");
        show_alarm_info();
    }
    if (strncmp(type, "sc", 2) == 0 && strncmp(key, "interval", 5) == 0) {//alarm only support interval
        sc = get_dev_sc_alarm_info_by_phase(p);
        sc->interval = (uint16_t)f_value;
        update_dev_sc_alarm_info(p, sc);

        LOG_INFO("Show alarm info:");
        show_sc_alarm_info(p);
    }
    else if (strncmp(type, "product", 7) == 0) {
        product = get_dev_product_info();
        if (strncmp(key, "date", 4) == 0) {
            product->date = (uint32_t)f_value;
        }
        if (strncmp(key, "id", 2) == 0) {
            snprintf((char*)product->id, sizeof(product->id), "%s", s_value);
        }

        update_dev_product_info(product);
        LOG_INFO("Show product info:");
        show_dev_product_info();
    }
    else if (strncmp(type, "self", 4) == 0) {//4G unsupport
    	LOG_INFO("4G unsupport");
    }
    else if (strncmp(type, "server", 6) == 0) {
        server =  (get_dev_server_info());
        if (strncmp(key, "port", 4) == 0) {
            server->port = (uint16_t)f_value;
        }
        if (strncmp(key, "ip", 2) == 0) {
            ip_section[0] = strtok(s_value, (char*)&delimiters);
            i = 1;
            while (NULL != (ip_section[i] = strtok(NULL, (char*)&delimiters))) {
               i++;
            }
            server->ip[0] = strtol(ip_section[0], NULL, 10);
            server->ip[1] = strtol(ip_section[1], NULL, 10);
            server->ip[2] = strtol(ip_section[2], NULL, 10);
            server->ip[3] = strtol(ip_section[3], NULL, 10);
        }

        update_dev_server_info(server);
        LOG_INFO("Show server info:");
        show_dev_server_info();
    }
    else if ((0 == strncmp(type, "cur", 3)) || (0 == strncmp(type, "vol", 3)) || (0 == strncmp(type, "temp", 4)) || (0 == strncmp(type, "hfct", 4))) {
        update_mx_cfg_by_type_and_phase(type, key, p, f_value, s_value);
    }
    else {
    	LOG_INFO("Unsupported cfg type!");
        return 1;
    }

    update_saved_dev_cfg();
    optind = 1;
    return 0;
}

void reset_cfg_command(void)
{
    set_default_dev();
    update_saved_dev_cfg();
}

void test_inner_flash_command(void)
{
    test_inner_flash();
}

void test_saved_history_data(void)
{
//    mx_sensor_info_t* mx;
//    mx_history_data_t data;
//
//    mx = get_dev_cur_info_by_phase(PHASE_A);
//
//    mx->max = 9999.9;
//    mx->min = 0;
//    mx->realtime = 1;
//    mx->timestamp = rtt_get_counter();
//    append_cur_history_data(*mx, PHASE_A);
//    mx->realtime = 2;
//    append_cur_history_data(*mx, PHASE_A);
//    mx->realtime = 3;
//    append_cur_history_data(*mx, PHASE_A);
//
//    data = query_cur_history_data(PHASE_A, 0);
//    LOG_INFO("History data: %d: max:%d, min:%d, realtime:%d", 0, (uint32_t)data.max, (uint32_t)data.min, (uint32_t)data.realtime);
//
//    data = query_cur_history_data(PHASE_A, 1);
//    LOG_INFO("History data: %d: max:%d, min:%d, realtime:%d", 1, (uint32_t)data.max, (uint32_t)data.min, (uint32_t)data.realtime);
//
//    data = query_cur_history_data(PHASE_A, 2);
//    LOG_INFO("History data: %d: max:%d, min:%d, realtime:%d", 2, (uint32_t)data.max, (uint32_t)data.min, (uint32_t)data.realtime);
}



void read_history_by_idx(sensor_t type, phase_t phase, uint32_t idx)
{
	(void)type;
	(void)phase;
	(void)idx;
//    mx_history_data_t mx = { 0 };
//    st_history_data_t st = { 0 };
//    sc_history_data_t sc = { 0 };
//
//    history_count_t count = get_history_count_info(type, phase);
//    LOG_INFO("Get history count for %s %s as: total:%d, index:%d", get_type_desc(type), get_phase_desc(phase), count.total_num, count.cur_index);
//
//    switch (type) {
//    case CUR:
//        mx = query_cur_history_data(phase, idx);
//        break;
//    case VOL:
//        mx = query_vol_history_data(phase, idx);
//        break;
//    case TEMP:
//        mx = query_temp_history_data(phase, idx);
//        break;
//    case HFCT:
//        mx = query_hfct_history_data(phase, idx);
//        break;
//    case ALARM:
//        st = query_alarm_history_data(idx);
//        break;
//    case SC_ALARM:
//        sc = query_sc_alarm_history_data(phase, idx);
//        break;
//    }
//
//    if (type == ALARM) {
//        LOG_INFO("Get history data for ALARM as: defense:0x%04x, status:0x%04x, timestamp:%ld", st.defense, st.status, st.timestamp);
//    }
//    else if (type == SC_ALARM) {
//        LOG_INFO("Get history data for %s %s as: current:0x%04x, status:0x%04x, timestamp:%ld", get_type_desc(type), get_phase_desc(phase), sc.current, sc.alarm, st.timestamp);
//    }
//    else {
//        LOG_INFO("Get history data for %s %s as: max:%d, min:%d, realtime:%d, timestamp:%ld",
//                 get_type_desc(type), get_phase_desc(phase), (uint32_t )mx.max, (uint32_t )mx.min,(uint32_t )mx.realtime, mx.timestamp);
//    }
}

int history_cfg_command(int argc, char **argv)
{
    int opt;
    char type[20] = {0};
    phase_t p = PHASE_A;
    uint32_t idx = 0;

    static const struct option long_opts[] = {
            { "help", no_argument, NULL, 'h' },
            { "mock", required_argument, NULL, 'm' },
            { "type", required_argument, NULL, 't' },
            { "phase", required_argument, NULL, 'p' },
            { "index", required_argument, NULL, 'i' },
            { NULL, 0, NULL, 0 },
    };

    if (argc < 2) {
        history_cfg_uasge();
        return 1;
    }

    while ((opt = getopt_long(argc, argv, "hmt:p:i:", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            history_cfg_uasge();
            return 0;
        case 'm':
            test_saved_history_data();
            return 0;
            break;
        case 't':
            strcpy(type, optarg);
            break;
        case 'p':
            p = get_phase_by_char(*optarg);
            break;
        case 'i':
            idx = strtol(optarg, NULL, 10);
            break;
        default:
        	LOG_INFO("Unsupported read option.");
            return 0;
            break;
        }
    }

    read_history_by_idx(get_type_by_string(type), p, idx);

    return 0;
}

void set_dev_timestamp(char* timestamp)
{
    uint32_t seconds = 0;

    seconds = (uint32_t)strtol(timestamp, NULL, 10);
    rtt_set_counter(seconds);

    LOG_INFO("Get current timestamp!");
}

int cfg_command(int argc, char **argv)
{
    if (argc < 2) {
        cfg_usage_help();
        return 1;
    }

    if (strncmp(argv[1], "read", 4) == 0) {
        read_cfg_command(argc - 1, argv + 1);
    }
    else if (strncmp(argv[1], "write", 5) == 0) {
        write_cfg_command(argc - 1, argv + 1);
    }
    else if (strncmp(argv[1], "reset", 5) == 0) {
        reset_cfg_command();
    }
    else if (strncmp(argv[1], "test", 5) == 0) {
        test_inner_flash_command();
    }
    else if (strncmp(argv[1], "saveenv", 7) == 0) {
        update_saved_dev_cfg();
    }
    else if (strncmp(argv[1], "loadenv", 7) == 0) {
        load_saved_dev_cfg();
    }
    else if (strncmp(argv[1], "history", 7) == 0) {
        history_cfg_command(argc - 1, argv + 1);
    }
    else if ((strncmp(argv[1], "time", 4) == 0) && (argc == 3)) {
        set_dev_timestamp(argv[2]);
    }
    else if((strncmp(argv[1], "count", 5) == 0))
    {
    	show_history_data_count();
    }
    optind = 1;
    return 0;
}
