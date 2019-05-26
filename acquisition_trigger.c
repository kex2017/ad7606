/*
 * acquisition_trigger.c
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#include "log.h"
#include "msg.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/rtt.h"

#include "acquisition_trigger.h"
#include "gps_sync.h"
#include "daq.h"
#include "cable_ground_threads.h"

typedef struct _channel_sample_cfg {
    uint32_t sample_timestamp;
    channel_cfg_t cfg;
} channel_sample_cfg_t;


/* used to debug, print log beside IRQ handler */
static uint32_t reg_written_flag = FLAG_OFF;

/* time to trigger FPGA sample data*/
static uint32_t master_sample_time = 0;

#define BASE_CHANNEL 0X0010  //bit4~bit13分别表示0~9通道
static uint16_t g_data_sample_flag = 0X0000; //should be 0x0000, here set 0X0010 to test read channel 0 data from FPGA

static channel_sample_cfg_t g_channel_sample_cfg[MAX_CHANNEL] = { {0} };
void open_pdirq_timer_trigger(uint32_t periodic_value);
void open_ovirq_timer_trigger(uint32_t periodic_value);
#define JF_DEFAULT_GAIN (45)
void init_sample_cfg(void)
{
    uint8_t i = 0;
    channel_cfg_t *cfg = NULL;

    for (i = 0; i < MAX_CHANNEL; i++) {
        cfg = &g_channel_sample_cfg[i].cfg;
        cfg->channel = i;
        cfg->gain = 10;
        cfg->max_fre = 30;
        cfg->min_fre = 10;
        cfg->threshold = 100;
    }
}
void display_sample_info(void)
{
    uint8_t i = 0;
    channel_cfg_t cfg;

    LOG_DEBUG("Next sample time: %ld for channel:%04x", master_sample_time, g_data_sample_flag);
    for (i = 0; i < MAX_CHANNEL; i++) {
        cfg = g_channel_sample_cfg[i].cfg;
        LOG_DEBUG("Channel:%d, max_fre:%d, min_fre:%d, gain:%d, threshold:%d.", cfg.channel, cfg.max_fre, cfg.min_fre, cfg.gain, cfg.threshold);
    }
}

/***********************************************************
 * set voltage and current sample parameters
 * *********************************************************/
void set_channel_sample_cfg(channel_cfg_t cfg_info)
{
    g_channel_sample_cfg[cfg_info.channel].cfg = cfg_info;
}

channel_cfg_t get_channel_sample_cfg(uint8_t channel)
{
    return g_channel_sample_cfg[channel].cfg;
}

void set_channel_gain_cfg(uint8_t channel, uint8_t gain)
{
	g_channel_sample_cfg[channel].cfg.gain = gain;
}

uint8_t get_channel_gain_cfg(uint8_t channel)
{
	return g_channel_sample_cfg[channel].cfg.gain;
}


/*************************************************************
 * 局部放电事件中断标志清除周期
 * ***********************************************************/
static uint32_t g_pd_and_ov_periodic_value = 5;//TODO:周期等待确认
void set_event_check_periodic_value(uint32_t value)
{
    g_pd_and_ov_periodic_value = value;
}

uint32_t get_event_check_periodic_value(void)
{
    return g_pd_and_ov_periodic_value;
}

/*************************************************************
 * 局部放电事件中断设置信息函数
 * ***********************************************************/
static int g_pd_reset_times = 0;
static msg_read_info_t g_pd_info = {0};
void get_reset_pd_flag_times(void)
{
    printf("PD flag reset times is :%d\r\n", g_pd_reset_times);
}

int partial_discharge_event_happen_notify(void)
{
    static uint32_t last_timestamp = 0;
    uint32_t cur_timestamp = 0;
    msg_t g_pd_msg;

    cur_timestamp = rtt_get_counter();
    disable_partial_discharge_irq();
    if (cur_timestamp - last_timestamp < get_event_check_periodic_value()) { //check PD event every 3 second
        daq_fsmc_pd_flag_reset(0x01C0);//TODO:等待确认是否需要reset
        open_pdirq_timer_trigger(g_pd_and_ov_periodic_value);
        g_pd_reset_times++;
        return 0;
    }

    if (cur_timestamp < 3600) { //device time is 1970, shouldn't check PD event
        open_pdirq_timer_trigger(g_pd_and_ov_periodic_value);
        return 0;
    }

    last_timestamp = cur_timestamp;
    g_pd_msg.type = MSG_READ_PD;
    g_pd_info.channels = 0x1C00;
    g_pd_info.timestamp = rtt_get_counter();
    g_pd_msg.content.ptr = &g_pd_info;
    msg_send_int(&g_pd_msg, data_acquisition_service_pid);

    return 0;
}

/****************************************************************************
 * PARTIAL_DISCHARGE_EXIT15_10_IRQHandler
 * 局部放电中断处理函数
 *******************************************************************************/
void PARTIAL_DISCHARGE_EXIT15_10_IRQHandler(void *arg)
{
    (void)arg;
    if(NVIC_GetActive(EXTI15_10_IRQn) != 0) {
        NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
        partial_discharge_event_happen_notify();
    }
}

/*************************************************************
 * 局部放电事件及过压事件中断初始化
 * ***********************************************************/
void pd_and_ov_event_irq_init(void)
{
    static int init_flag = 0;

    if (!init_flag) {
        LOG_INFO("Enable PD and OV IRQ...");
        over_voltage_event_pins_init();
        enable_over_voltage_irq();
        partial_discharge_event_pins_init();
        enable_partial_discharge_irq();
        init_flag = 1;
    }
    else {
        LOG_INFO("PD and OV IRQ inited already.");
    }
}

/*************************************************************
 * 过压事件中断设置信息函数
 * @brief   Notify that over voltage event happen.
 *          Usually called by hardware PIN interrupt.
 *
 * @return  negative value on error;
 *          FPGA not ready etc...
 * ***********************************************************/
static int g_ov_reset_times = 0;
static msg_read_info_t g_ov_info = {0};
void get_reset_ov_flag_times(void)
{
    printf("OV flag reset times is :%d\r\n", g_ov_reset_times);
}

int over_voltage_event_happen_notify(void)
{
    static uint32_t last_timestamp = 0;
    uint32_t cur_timestamp = 0;
    msg_t g_ov_msg;

    cur_timestamp = rtt_get_counter();
    disable_over_voltage_irq();
    if (cur_timestamp < 3600 || cur_timestamp - last_timestamp < get_event_check_periodic_value()) { //check PD event every 3 second
        daq_spi_ov_flag_reset(0);//TODO:待确认过压事件发生标志的清除如何实现
        open_ovirq_timer_trigger(g_pd_and_ov_periodic_value);
        g_ov_reset_times++;
        return 0;
    }

    last_timestamp = cur_timestamp;
    g_ov_msg.type = MSG_READ_OV;
    g_ov_info.channels = 0x07;
    g_ov_info.timestamp = rtt_get_counter();
    g_ov_msg.content.ptr = &g_ov_info;

    msg_send_int(&g_ov_msg, data_acquisition_service_pid);

    return 0;
}
/*******************************************************************************
 * OVER_VOLTAGE_EXIT2_IRQHandler
 * 过电压中断处理函数
 *******************************************************************************/
void OVER_VOLTAGE_EXIT2_IRQHandler(void *arg)
{
    (void)arg;
    if(NVIC_GetActive(EXTI2_IRQn) != 0) {
        NVIC_ClearPendingIRQ(EXTI2_IRQn);
        over_voltage_event_happen_notify();
    }
}

uint32_t get_channel_sample_timestamp(uint8_t channel)
{
    return g_channel_sample_cfg[channel].sample_timestamp;
}

void set_channel_sample_timestamp(uint8_t channel, uint32_t timestamp)
{
    g_channel_sample_cfg[channel].sample_timestamp = timestamp;
}

void generate_pulse_signal(void)
{
//    launch_pulse_signal(1);
}
/*******************************************************************************
 * GPS_EXIT3_IRQHandler
 * gps秒脉冲中断处理函数
 *******************************************************************************/
void GPS_EXIT3_IRQHandler(void *arg)
{
    (void)arg;
//    /**call launch_pulse_signal every seconds**/
//    generate_pulse_signal();
////    set_fpga_utc_time(rtt_get_counter());//test set time to fpga 1s
//    if (get_sample_timestamp() == rtt_get_counter()) {
//        reg_written_flag = FLAG_ON;
//        daq_fsmc_write_reg(TRIGGER_REG, FLAG_ON);
//        clear_sample_timestamp();
//    }
}
void trigger_signal_by_hand(void)
{
    reg_written_flag = FLAG_ON;
    daq_fsmc_write_reg(TRIGGER_REG, FLAG_ON);

    LOG_INFO("Trigger to write register(8) for FPGA by hand.");
}

void trigger_sample(void)
{
    uint8_t channels[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };//FIXME: Chen PRPD need to trigger??
    daq_fsmc_multi_chans_sample(channels, sizeof(channels));
}

static xtimer_t g_trigger_timer, g_manual_trigger_timer, g_open_pdirq_timer, g_open_ovirq_timer;
static uint32_t g_periodic_value = 2 * US_PER_SEC;
/***********************************************************
 *  Never add log in this function and sub function
 *  run in ISR context cause ISR stack overflow
 * *********************************************************/
static msg_read_info_t g_common_info = {0};
void _callback_trigger_sample_reset_timer(void *arg)
{
    (void)arg;
    msg_t msg;

    msg.type = MSG_READ_COMMON_AND_PRPD;
    g_common_info.channels = 0xE3FF; //0-9, 13-15 channels
    g_common_info.timestamp = rtt_get_counter();
    msg.content.ptr = &g_common_info;
    trigger_sample();   //shouldn't use mutex in this function because it's called in IRQ

    msg_send_int(&msg, data_acquisition_service_pid);
    xtimer_set(&g_trigger_timer, g_periodic_value);
}

void _callback_open_pdirq_timer(void *arg)
{
    (void)arg;
    msg_t msg;

    msg.type = MSG_READ_PD;
    g_common_info.channels = 0x0;
    g_common_info.timestamp = rtt_get_counter();
    msg.content.ptr = &g_common_info;

    msg_send_int(&msg, data_acquisition_service_pid);
}

void _callback_open_ovirq_timer(void *arg)
{
    (void)arg;
    msg_t msg;

    msg.type = MSG_READ_OV;
    g_common_info.channels = 0x0;
    g_common_info.timestamp = rtt_get_counter();
    msg.content.ptr = &g_common_info;

    msg_send_int(&msg, data_acquisition_service_pid);
}

void _callback_manual_trigger_sample_reset_timer(void *arg)
{
    msg_t msg;

    msg.type = *(uint16_t*)arg;
    g_common_info.channels = 0xE3FF; //0-9, 13-15 channels
    g_common_info.timestamp = rtt_get_counter();
    msg.content.ptr = &g_common_info;
    trigger_sample();

    msg_send_int(&msg, data_acquisition_service_pid);
}

static uint16_t g_data_type = MSG_READ_COMMON_AND_PRPD;
void set_manual_sample_timer_trigger(uint32_t seconds, uint16_t data_type)
{
    g_data_type = data_type;
    g_manual_trigger_timer.arg = (void*)&g_data_type;
    g_manual_trigger_timer.callback = _callback_manual_trigger_sample_reset_timer;
    xtimer_set(&g_manual_trigger_timer, (seconds * US_PER_SEC));
}

void start_sample_timer_trigger(void)
{
    g_trigger_timer.callback = _callback_trigger_sample_reset_timer;
    xtimer_set(&g_trigger_timer, g_periodic_value);
}
void stop_sample_timer_trigger(void)
{
    xtimer_remove(&g_trigger_timer);
}

void open_pdirq_timer_trigger(uint32_t periodic_value)
{
    g_open_pdirq_timer.callback = _callback_open_pdirq_timer;
    xtimer_set(&g_open_pdirq_timer, periodic_value * US_PER_SEC);
}

void remove_pdirq_timer(void)
{
    xtimer_remove(&g_open_pdirq_timer);
}

void open_ovirq_timer_trigger(uint32_t periodic_value)
{
    g_open_ovirq_timer.callback = _callback_open_ovirq_timer;
    xtimer_set(&g_open_ovirq_timer, periodic_value * US_PER_SEC);
}

void remove_ovirq_timer(void)
{
    xtimer_remove(&g_open_ovirq_timer);
}


