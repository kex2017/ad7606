#include <stdint.h>
#include "period_data.h"
#include "fault_location_threads.h"
#include "frame_encode.h"
#include "frame_handler.h"
#include "thread.h"
#include "periph/rtt.h"
#include "env_cfg.h"
#include "x_delay.h"
#include "type_alias.h"
#include "periph/rtt.h"
#include "data_send.h"
#include "over_current.h"
#include "upgrade_from_flash.h"
#include "periph/flashpage.h"
#include "low_power.h"
#include "periph/stm32f10x_std_periph.h"

static kernel_pid_t data_send_pid;

static uint32_t period_send_data_time_start = 0;
static uint32_t period_send_data_time_now = 0;

static uint32_t bat_do_sample_time_start = 0;
static uint32_t bat_do_sample_time_now = 0;
static uint16_t bat;

uint16_t bat_val_channel = 5;

void period_data_hook(kernel_pid_t pid)
{
    data_send_pid = pid;
}

#define OVER_CURRENT_CURVE_BASE_PAGE (PROGRAM_FPGA_BASE_ADDR-CPU_FLASH_BASE_ADDR)/FLASH_PAGE_SIZE
#define CURVE_DATA_PAGE_NUM (4)//8k

int get_curve_page_addr(uint8_t phase, uint8_t channel)
{
    return (OVER_CURRENT_CURVE_BASE_PAGE + CURVE_DATA_PAGE_NUM * (phase * OVER_CURRENT_CHANNEL_COUNT + channel));
}

void save_curve_data(uint8_t phase, uint8_t channel, uint8_t *data, uint32_t data_len)
{
    uint8_t page_num = data_len/FLASH_PAGE_SIZE + !!(data_len%FLASH_PAGE_SIZE);
    int page = get_curve_page_addr(phase, channel);
    uint16_t *page_addr = flashpage_addr(page);

    for(uint8_t i = 0; i < page_num; i++){
        flashpage_write(page+i, NULL);//erase
    }

    flashpage_write_raw(page_addr, data, data_len);
}

void read_curve_data(uint8_t phase, uint8_t channel, uint32_t data_addr, uint8_t *data, uint32_t data_len)
{
    int page = get_curve_page_addr(phase, channel);
    uint16_t *page_addr = flashpage_addr(page) + data_addr;

    memcpy(data, page_addr, data_len);
}

float get_bat_vol(void)
{
    return CAL_VOL(bat);
}

static void bat_sample_init(void)
{
    adc3_sample_init(&bat_val_channel, 1);
}

void interval_do_bat_sample(void)
{
    bat_do_sample_time_now = rtt_get_counter();
    if (bat_do_sample_time_now - bat_do_sample_time_start > BAT_SAMPLE_INTERVAL) {
        bat_sample_init();
        bat = adc3_sample_by_channel(bat_val_channel);
        LOG_INFO("battery voltage adc is %d value is %f\r\n", bat, CAL_VOL(bat));
        change_pw_mode_by_bat_level();
        bat_do_sample_time_start = bat_do_sample_time_now;
    }
}


static void interval_send_period_data(uint16_t interval_time)
{
    msg_t msg;
    period_send_data_time_now = rtt_get_counter();
    if (period_send_data_time_start == 0) period_send_data_time_start = period_send_data_time_now;
    if (period_send_data_time_now - period_send_data_time_start > interval_time) {

        msg.type = PERIOD_DATA_TYPE;
        msg.content.value = rtt_get_counter();
        msg_send(&msg, data_send_pid);

        period_send_data_time_start = period_send_data_time_now;
    }
}


void *period_data_serv(void *arg)
{
    (void)arg;

    uint16_t* interval_time = cfg_get_device_data_interval();

    while (1) {
//        interval_do_bat_sample();
        interval_send_period_data(*interval_time);
        delay_ms(200);
    }
}

static char period_data_thread_stack[THREAD_STACKSIZE_MAIN/2];
kernel_pid_t period_data_serv_init(void)
{
    kernel_pid_t _pid = thread_create(period_data_thread_stack, sizeof(period_data_thread_stack),
                                      PERIOD_DATA_PRIORITY,
                                      THREAD_CREATE_STACKTEST, period_data_serv, NULL, "period data serv");
    return _pid;
}
