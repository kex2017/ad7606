/*
 * history_data.c
 *
 *  Created on: Mar 26, 2019
 *      Author: chenzy
 */

#include "periph/flashpage.h"

#include "log.h"
#include "string.h"

#include "history_data.h"

#define MX_SENSOR_DATA_SIZE (16)  //timestamp, max, min, realtime
#define MAX_MX_HISTORY_DATA_NUM (128*5)  //5page

#define ST_SENSOR_DATA_SIZE (8)   //timestamp, defense, status
#define MAX_ST_HISTORY_DATA_NUM (256*5)  //5page

#define SINGLE_PAGE_MX_COUNT (128)
#define SINGLE_PAGE_ST_COUNT (256)

#define DEV_HISTORY_DATA_BASE_ADDR     0x08050000  //160k
#define CPU_FLASH_BASE_ADDR           0x08000000  //512k


uint32_t get_write_page(sensor_t type, phase_t phase, history_count_t info)
{
    uint32_t addr = 0;
    uint32_t page = 0;

    addr = DEV_HISTORY_DATA_BASE_ADDR + type * 15 * FLASH_PAGE_SIZE + phase * FLASH_PAGE_SIZE;//15 = 3 * 5, single phase cost 5 page
    page = (addr - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    if (type == ALARM || type == SC_ALARM) {
        page += info.cur_index / SINGLE_PAGE_ST_COUNT;
    }
    else {
        page += info.cur_index / SINGLE_PAGE_MX_COUNT;
    }

    return page;
}

uint32_t get_read_page(sensor_t type, phase_t phase, uint32_t cur_index)
{
    uint32_t addr = 0;
    uint32_t page = 0;

    addr = DEV_HISTORY_DATA_BASE_ADDR + type * 15 * FLASH_PAGE_SIZE + phase * FLASH_PAGE_SIZE;//15 = 3 * 5, single phase cost 5 page
    page = (addr - CPU_FLASH_BASE_ADDR) / FLASH_PAGE_SIZE;
    if (type == ALARM || type == SC_ALARM) {
        page += cur_index / SINGLE_PAGE_ST_COUNT;
    }
    else {
        page += cur_index / SINGLE_PAGE_MX_COUNT;
    }

    return page;
}

/*************************************************************************
 * operations for device current history data count info
*************************************************************************/
history_count_t get_history_count_info(sensor_t type, phase_t phase)
{
    history_count_t info = {0};
    dev_hisroty_count_t *history = {0};

    history = get_dev_history_count();
    switch (type) {
    case CUR:
        info = history->cur[phase];
        break;
    case VOL:
        info = history->vol[phase];
        break;
    case TEMP:
        info = history->temp[phase];
        break;
    case HFCT:
        info = history->hfct[phase];
        break;
    case SC_ALARM:
        info = history->sc_alarm[phase];
        break;
    case ALARM:
        info = history->alarm;
        break;
    default:
        LOG_INFO("Invalid sensor type: %d", type);
        break;
    }

    return info;
}

void increase_history_count(sensor_t type, phase_t phase, history_count_t info)
{
    dev_hisroty_count_t* history = {0};
    history = get_dev_history_count();


//	if (info.total_num < MAX_ST_HISTORY_DATA_NUM) {
		info.total_num += 1;
//	}
//	if (type == ALARM || type == SC_ALARM) {
//		info.cur_index = (info.cur_index + 1) % MAX_ST_HISTORY_DATA_NUM;
//	} else {
		info.cur_index = (info.cur_index + 1) % MAX_MX_HISTORY_DATA_NUM;
//	}

    switch (type) {
    case CUR:
        history->cur[phase] = info;
        break;
    case VOL:
        history->vol[phase] = info;
        break;
    case TEMP:
        history->temp[phase] = info;
        break;
    case HFCT:
        history->hfct[phase] = info;
        break;
    case SC_ALARM:
        history->sc_alarm[phase] = info;
        break;
    case ALARM:
        history->alarm = info;
        break;
    default:
        LOG_INFO("Invalid sensor type: %d", type);
        break;
    }

    update_dev_history_count(history);
}

void append_mx_hitory_data(sensor_t type, phase_t phase, const mx_sensor_info_t* mx)
{
    uint32_t page = 0;
    history_count_t info = { 0 };
    mx_history_data_t mx_history_data[SINGLE_PAGE_MX_COUNT] = { { 0 } };

    info = get_history_count_info(type, phase);

    page = get_write_page(type, phase, info);
    memcpy((void*)mx_history_data, (void*)(page * FLASH_PAGE_SIZE + CPU_FLASH_BASE_ADDR), FLASH_PAGE_SIZE);

    mx_history_data[info.cur_index % SINGLE_PAGE_MX_COUNT].max = mx->max;
    mx_history_data[info.cur_index % SINGLE_PAGE_MX_COUNT].min = mx->min;
    mx_history_data[info.cur_index % SINGLE_PAGE_MX_COUNT].realtime = mx->realtime;
    mx_history_data[info.cur_index % SINGLE_PAGE_MX_COUNT].timestamp = mx->timestamp;

    flashpage_write(page, (void*)mx_history_data);
    increase_history_count(type, phase, info);
}

mx_history_data_t query_mx_hitory_data(sensor_t type, phase_t phase, uint32_t idx)
{
    uint32_t page = 0;
    mx_history_data_t mx_history_data[SINGLE_PAGE_MX_COUNT] = { { 0 } };

    page = get_read_page(type, phase, idx);
    memcpy((void*)mx_history_data, (void*)(page * FLASH_PAGE_SIZE + CPU_FLASH_BASE_ADDR), FLASH_PAGE_SIZE);

    return mx_history_data[idx % SINGLE_PAGE_MX_COUNT];
}

/*************************************************************************
 * operations for device current history data
*************************************************************************/
void append_cur_history_data(mx_sensor_info_t mx, phase_t phase)
{
    append_mx_hitory_data(CUR, phase, &mx);
}

mx_history_data_t query_cur_history_data(phase_t phase, uint32_t idx)
{
    return query_mx_hitory_data(CUR, phase, idx);
}

/*************************************************************************
 * operations for device voltage history data
*************************************************************************/
void append_vol_history_data(mx_sensor_info_t mx, phase_t phase)
{
    append_mx_hitory_data(VOL, phase, &mx);
}

mx_history_data_t query_vol_history_data(phase_t phase, uint32_t idx)
{
    return query_mx_hitory_data(VOL, phase, idx);
}

/*************************************************************************
 * operations for device temp history data
*************************************************************************/
void append_temp_history_data(mx_sensor_info_t mx, phase_t phase)
{
    append_mx_hitory_data(TEMP, phase, &mx);
}

mx_history_data_t query_temp_history_data(phase_t phase, uint32_t idx)
{
    return query_mx_hitory_data(TEMP, phase, idx);
}

/*************************************************************************
 * operations for device hfct history data
*************************************************************************/
void append_hfct_history_data(mx_sensor_info_t mx, phase_t phase)
{
    append_mx_hitory_data(HFCT, phase, &mx);
}

mx_history_data_t query_hfct_history_data(phase_t phase, uint32_t idx)
{
    return query_mx_hitory_data(HFCT, phase, idx);
}


/*************************************************************************
 * operations for device sc alarm history data
*************************************************************************/
void append_sc_alarm_history_data(sc_sensor_info_t sc, phase_t phase)
{
    uint32_t page = 0;
    history_count_t info = {0};
    sc_history_data_t sc_history_data[SINGLE_PAGE_ST_COUNT] = {{0}};

    info = get_history_count_info(SC_ALARM, phase);

    page = get_write_page(SC_ALARM, phase, info);
    memcpy((void*)sc_history_data, (void*)(page * FLASH_PAGE_SIZE + CPU_FLASH_BASE_ADDR), FLASH_PAGE_SIZE);

    sc_history_data[info.cur_index].current = sc.current;
    sc_history_data[info.cur_index].alarm = sc.alarm;
    sc_history_data[info.cur_index].timestamp = sc.timestamp;

    flashpage_write(page, (void*)sc_history_data);

    increase_history_count(SC_ALARM, phase, info);
}
sc_history_data_t query_sc_alarm_history_data(phase_t phase, uint32_t index)
{
    uint32_t page = 0;
    sc_history_data_t sc_history_data[SINGLE_PAGE_ST_COUNT] = {{0}};

    page = get_read_page(SC_ALARM, phase, index);

    memcpy((void*)sc_history_data, (void*)(page * FLASH_PAGE_SIZE + CPU_FLASH_BASE_ADDR), FLASH_PAGE_SIZE);

    return sc_history_data[index % SINGLE_PAGE_ST_COUNT];
}

/*************************************************************************
 * operations for device alarm history data
*************************************************************************/
void append_alarm_history_data(st_sensor_info_t st, phase_t phase)
{
    uint32_t page = 0;
    history_count_t info = {0};
    st_history_data_t st_history_data[SINGLE_PAGE_ST_COUNT] = {{0}};

    info = get_history_count_info(ALARM, phase);

    page = get_write_page(ALARM, phase, info);
    memcpy((void*)st_history_data, (void*)(page * FLASH_PAGE_SIZE + CPU_FLASH_BASE_ADDR), FLASH_PAGE_SIZE);

    st_history_data[info.cur_index].defense = st.defense;
    st_history_data[info.cur_index].status = st.status;
    st_history_data[info.cur_index].timestamp = st.timestamp;

    flashpage_write(page, (void*)st_history_data);

    increase_history_count(ALARM, phase, info);
}

st_history_data_t query_alarm_history_data(uint32_t index)
{
    uint32_t page = 0;
    st_history_data_t st_history_data[SINGLE_PAGE_ST_COUNT] = {{0}};

    page = get_read_page(ALARM, PHASE_A, index);
    memcpy((void*)st_history_data, (void*)(page * FLASH_PAGE_SIZE + CPU_FLASH_BASE_ADDR), FLASH_PAGE_SIZE);

    return st_history_data[index % SINGLE_PAGE_ST_COUNT];
}
