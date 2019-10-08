#include <stdint.h>
#include <string.h>
#include <math.h>

#include "over_current.h"
#include "upgrade_from_flash.h"
#include "fault_location_threads.h"
#include "env_cfg.h"
#include "log.h"
#include "board.h"
#include "periph/rtt.h"
#include "thread.h"
#include "x_delay.h"
#include "type_alias.h"
#include "periph/gpio.h"
#include "data_send.h"
#include "data_transfer.h"
#include "period_data.h"
#include "daq.h"
#include <math.h>

kernel_pid_t hf_over_current_pid = KERNEL_PID_UNDEF;

static kernel_pid_t data_send_pid;
void over_current_hook(kernel_pid_t pid)
{
    data_send_pid = pid;
}

/*******************over current event pin irq *******************/
#define  HF_OVER_CURRENT_EVENT_PIN  GPIO_PIN(PORT_E,2)

void enable_hf_over_current_irq(void)
{
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void disable_hf_over_current_irq(void)
{
    NVIC_DisableIRQ(EXTI2_IRQn);
}

void OVER_CURRENT_EXIT2_IRQHandler(void *arg)
{
    (void)arg;
    msg_t msg;
    if(NVIC_GetActive(EXTI2_IRQn) != 0) {
        NVIC_ClearPendingIRQ(EXTI2_IRQn);
        msg_send(&msg, hf_over_current_pid);
    }
}

void hf_over_current_event_pins_init(void)
{
    gpio_clear(HF_OVER_CURRENT_EVENT_PIN);
    gpio_init_int(HF_OVER_CURRENT_EVENT_PIN, GPIO_IN, GPIO_RISING, OVER_CURRENT_EXIT2_IRQHandler, NULL);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    disable_hf_over_current_irq();
}

void init_hf_over_current_irq(void)
{
    hf_over_current_event_pins_init();
    enable_hf_over_current_irq();
}

uint16_t get_fpga_uint16_data(uint16_t data)
{
    uint16_t fpga_data = 0;
    uint16_t data_h = (data >> 8)&0xFF;
    uint16_t data_l = data & 0xFF;

    fpga_data = data_l << 8 | data_h;
    return fpga_data;
}

/********************over current event******************************/
hf_over_current_info_t g_hf_over_current_info[MAX_PHASE][MAX_HF_OVER_CURRENT_CHANNEL_COUNT] = { {{0} }};
pf_over_current_info_t g_pf_over_current_info[MAX_PHASE][MAX_PF_OVER_CURRENT_CHANNEL_COUNT] = { {{0}} };

over_current_status_t over_current_status[MAX_PHASE][OVER_CURRENT_CHANNEL_COUNT] = {0};
over_current_data_t g_over_current_data[MAX_PHASE][OVER_CURRENT_CHANNEL_COUNT+1] = {0};
uint16_t curve_data[MAX_FPGA_DATA_LEN/2];
uint32_t g_hf_max[FPGA_PHASE_NUM][MAX_HF_OVER_CURRENT_CHANNEL_COUNT] = { 0 };
float g_pf_cur[FPGA_PHASE_NUM][MAX_PF_OVER_CURRENT_CHANNEL_COUNT] = { 0 };

/******************hf over current info*******************/
void set_hf_over_current_threshold(uint8_t phase, uint8_t channel, uint16_t threshold)
{
    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    change_spi_cs_pin(phase);
    daq_spi_set_hf_threshold(channel, threshold);
    g_hf_over_current_info[phase][channel].threshold = threshold;
    change_spi_cs_pin(cur_fpga_cs);
}

void set_hf_over_current_changerate(uint8_t phase, uint8_t channel, uint16_t changerate)
{
    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    change_spi_cs_pin(phase);
    daq_spi_set_hf_change_rate(channel, changerate);
    g_hf_over_current_info[phase][channel].change_rate = changerate;
    change_spi_cs_pin(cur_fpga_cs);
}

hf_over_current_info_t *get_hf_over_current_info(uint8_t phase, uint8_t channel)
{
    return &g_hf_over_current_info[phase][channel];
}

uint8_t check_over_current_status_is_free(uint8_t phase, uint8_t channel)
{
    if (1 == over_current_status[phase][channel].happened_flag) {
        if (1 == over_current_status[phase][channel].send_over_flag) {
            return STATUS_FREE;
        }
        else {
            return STATUS_BUSY;
        }
    }
    else
        return STATUS_FREE;
}

void set_over_current_happened_flag(uint8_t phase, uint8_t channel, uint8_t happened_flag)
{
    over_current_status[phase][channel].happened_flag = happened_flag;
}

void set_over_current_send_over_flag(uint8_t phase, uint8_t channel, uint8_t send_over_flag)
{
    over_current_status[phase][channel].send_over_flag = send_over_flag;
}

/******************pf over current info*******************/
void set_pf_over_current_threshold(uint8_t phase, uint8_t channel, uint16_t threshold)
{
    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    change_spi_cs_pin(phase);
    daq_spi_set_pf_threshold(channel, threshold * threshold * 512);
    g_pf_over_current_info[phase][channel].threshold = threshold;
    change_spi_cs_pin(cur_fpga_cs);
}

pf_over_current_info_t *get_pf_over_current_info(uint8_t phase, uint8_t channel)
{
    return &g_pf_over_current_info[phase][channel];
}


/********************hf over current data******************/
int get_hf_over_current_max(uint8_t channel)
{
    return daq_spi_get_dat_max(channel);
}

int get_hf_over_current_avr(uint8_t channel)
{
    return daq_spi_get_dat_avr(channel);
}
/**********************************************************/


/********************pf over current data******************/
float get_pf_over_current_rms(uint8_t channel)
{
    int64_t pf_sum = daq_spi_get_pf_sum_data(channel);
    uint16_t sample_cnt = daq_spi_get_data_len(channel)/2;
    LOG_INFO("pf_sum channel %d is %ll", channel, pf_sum);

    return sqrt(pf_sum / (double)sample_cnt);
}
/**********************************************************/

uint32_t read_over_current_sample_length(uint8_t channel)
{
#if ENABLE_MOCK_DATA
    if (channel < 2)
        return 8 * 1024;
    else
        return 3 * 1024;
#else
    return daq_spi_get_data_len(channel);
#endif
}

uint32_t read_over_current_cnt_since_plus(uint8_t channel)
{
#if ENABLE_MOCK_DATA
    return channel + 100;
#else
    return daq_spi_chan_cnt_since_plus(channel);
#endif
}

uint32_t read_over_current_one_sec_clk_cnt(uint8_t channel)
{
#if ENABLE_MOCK_DATA
    (void)channel;
    return channel * 100;
#else
    return daq_spi_one_sec_clk_cnt(channel);
#endif
}

uint32_t read_over_current_event_utc(uint8_t channel)
{
#if ENABLE_MOCK_DATA
    (void)channel;
    return rtt_get_counter();
#else
    return daq_spi_chan_event_utc(channel);
#endif
}

#define PI 3.141592
void read_over_current_sample_data(uint8_t channel, uint8_t *data, uint32_t addr, size_t byte_len)
{
#if ENABLE_MOCK_DATA
    (void)addr;
    (void)channel;
    uint16_t *p_data = (uint16_t *)data;

    if(channel < 2){
        for(size_t i = 0; i < byte_len/2;i++){
            p_data[i] = (uint16_t)(4096 * sin(PI * 2 * i / 4096) + 4096 );//一个周期波形
            p_data[i] = get_fpga_uint16_data(p_data[i]);
        }
    }
    else {
        for (size_t i = 0; i < byte_len / 2; i++) {
            p_data[i] = (uint16_t)(4096 * sin(PI * 2 * i / (1536 / 3)) + 4096);//三个周期波形
            p_data[i] = get_fpga_uint16_data(p_data[i]);
        }
    }
#else
    daq_spi_sample_data_read(channel, data, addr, byte_len);
#endif
}

over_current_data_t *get_over_current_data(uint8_t phase, uint8_t channel)
{
    return &g_over_current_data[phase][channel];
}

/**************************end*********************/
uint16_t sample_done_flag = 0xFFF;
int check_over_current_sample_done(uint8_t channel)
{
#if ENABLE_MOCK_DATA
    (void)channel;
    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    return (sample_done_flag & (1 << (channel + cur_fpga_cs * 4)));
#else
    return daq_spi_sample_done_check(channel);
#endif
}

void clear_over_current_sample_done_flag(uint8_t channel)
{
#if ENABLE_MOCK_DATA
    (void)channel;
    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    sample_done_flag &= ~(1 << (channel + cur_fpga_cs * 4));
#else
    daq_spi_clear_data_done_flag(channel);
#endif
}

void trigger_sample_over_current_by_hand(void)
{

    fpga_cs_t cur_fpga_cs = get_cur_fpga_cs();
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin(phase);
        for (uint8_t channel = 0; channel < OVER_CURRENT_CHANNEL_COUNT; channel++) {
            daq_spi_trigger_sample(channel);
        }
    }
    change_spi_cs_pin(cur_fpga_cs);

    set_server_call_flag();
}

void update_cycle_data(uint8_t phase, uint8_t channel)
{
#if ENABLE_MOCK_DATA
    g_hf_max[phase][channel] = 4000 + phase*100 + channel*10;
    g_pf_cur[phase][channel-2] = 300.0 + phase*10 + channel;
#else
    if (channel < 2) {
        g_hf_max[phase][channel] = get_hf_over_current_max(channel);
    }
    else {
        uint64_t pf_sum = daq_spi_get_pf_sum_data(channel);
        uint16_t sample_cnt = daq_spi_get_data_len(channel) / 2;

        sample_cnt = sample_cnt ? sample_cnt : 1536;


        g_pf_cur[phase][channel - 2] = sqrt(pf_sum * 1.0 / (sample_cnt/3));
    }
#endif
}

uint32_t get_hf_max(uint8_t phase, uint8_t channel)
{
    return g_hf_max[phase][channel];
}

float get_pf_rms(uint8_t phase, uint8_t channel)
{
    return g_pf_cur[phase][channel];
}

void set_default_threshold_rate(void)
{
    for (uint8_t phase = 0; phase < 3; phase++) {
        change_spi_cs_pin(phase);

        for (int channel = 0; channel < OVER_CURRENT_CHANNEL_COUNT; channel++) {
            if (channel < 2) {
                set_hf_over_current_threshold(phase, channel, cfg_get_device_hf_channel_threshold(phase, channel));
                set_hf_over_current_changerate(phase, channel, cfg_get_device_hf_channel_changerate(phase, channel));
                LOG_INFO("Set hf over current threshold and changerate for phase %s Channel %d: %d ,%d", (phase==0)?"A":(phase==1)?"B":"C", channel, cfg_get_device_hf_channel_threshold(phase, channel), cfg_get_device_hf_channel_changerate(phase, channel));
            }
            else{
                set_pf_over_current_threshold(phase, channel, cfg_get_device_pf_channel_threshold(phase, channel));
                LOG_INFO("Set pf over current threshold for phase %s Channel %d: %d", (phase==0)?"A":(phase==1)?"B":"C", channel, cfg_get_device_pf_channel_threshold(phase, channel));
            }
        }
    }
}

static void *hf_pf_over_current_event_service(void *arg)
{
    (void)arg;
    uint8_t channel = 0;
    uint32_t length = 0;
    uint8_t send_type = 0;
    msg_t msg;
    send_curve_info_t curve_info[FPGA_PHASE_NUM][OVER_CURRENT_CHANNEL_COUNT];

    set_default_threshold_rate();
    while (1) {
        send_type = get_send_type();
        for (uint8_t phase = 0; phase < 3; phase++) {
            if(CFG_NOK == check_fpga_cfg_status(phase))continue;
            change_spi_cs_pin(phase);
            for (channel = 0; channel < OVER_CURRENT_CHANNEL_COUNT; channel++) {
                if (check_over_current_sample_done(channel) && check_over_current_status_is_free(phase, channel)) {
                    set_over_current_happened_flag(phase, channel, 1);
                    set_over_current_send_over_flag(phase, channel, 0);
                    if ((length = read_over_current_sample_length(channel)) > MAX_FPGA_DATA_LEN) {
                        LOG_WARN("Get %s over current curve phase %s channel %d data length:%ld > MAX_FPGA_DATA_LEN, ignore it.", (channel<2)?"hf":"pf", (phase==0)?"A":(phase==1)?"B":"C", channel, (length));
                        continue;
                    }
                    memset(&g_over_current_data[phase][channel], 0, sizeof(over_current_data_t));
                    memset(curve_data, 0, sizeof(curve_data));

                    g_over_current_data[phase][channel].curve_len = length;
                    g_over_current_data[phase][channel].data_type = (channel<2)?HF_TYPE:PF_TYPE;
                    g_over_current_data[phase][channel].timestamp = read_over_current_event_utc(channel);
                    g_over_current_data[phase][channel].one_sec_clk_cnt = read_over_current_one_sec_clk_cnt(channel);
                    g_over_current_data[phase][channel].ns_cnt = read_over_current_cnt_since_plus(channel);
                    read_over_current_sample_data(channel, (uint8_t*)curve_data, 0, length);
                    save_curve_data(phase, channel, (uint8_t*)curve_data, length);
                    LOG_INFO("phase %s channel %d utc reg value %d", (phase==0)?"A":(phase==1)?"B":"C", channel, g_over_current_data[phase][channel].timestamp);
#if ENABLE_DEBUG
//                    memset(curve_data, 0, sizeof(curve_data));
//                    read_curve_data(phase, channel, 0, (uint8_t*)curve_data, length);
                    printf("phase %s channel %d read data is :\r\n", (phase==0)?"A":(phase==1)?"B":"C", channel);
                    for (uint16_t i = 0; i < length / 2; i++) {
                        curve_data[i] = get_fpga_uint16_data(curve_data[i]);
                        if (((i + 1) % 20) == 0) printf("\r\n");
                        printf("%d ", curve_data[i]);
                    }
                    printf("\r\n");

                    LOG_INFO("phase %s channel %d utc reg value %d", (phase==0)?"A":(phase==1)?"B":"C", channel, g_over_current_data[phase][channel].timestamp);
                    LOG_INFO("phase %s channel %d cnt_since_plus reg value %d", (phase==0)?"A":(phase==1)?"B":"C", channel, g_over_current_data[phase][channel].ns_cnt);
                    LOG_INFO("phase %s channel %d one_sec_clk_cnt reg value %d", (phase==0)?"A":(phase==1)?"B":"C", channel, g_over_current_data[phase][channel].one_sec_clk_cnt);
                    LOG_INFO("phase %s channel %d length is %d ns cnt is %ld", (phase==0)?"A":(phase==1)?"B":"C", channel, length, g_over_current_data[phase][channel].ns_cnt);
#endif
                    msg.type = MUTATION_TYPE;
                    curve_info[phase][channel].phase = phase;
                    curve_info[phase][channel].channel = channel;
                    curve_info[phase][channel].send_type = send_type;
                    msg.content.ptr = (void*)&curve_info[phase][channel];
                    msg_try_send(&msg, data_send_pid);
//                    send_over_current_curve(&g_over_current_data, channel, send_type);
//                    memset(&g_over_current_data, 0, sizeof(over_current_data_t));
                    clear_over_current_sample_done_flag(channel);
                }
//                else{
                    update_cycle_data(phase, channel);
//                }
            }
        }
        delay_ms(200);
    }
    return NULL;
}

static char hf_pf_over_current_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t hf_pf_over_current_service_init(void)
{
	if (hf_over_current_pid == KERNEL_PID_UNDEF) {
		hf_over_current_pid = thread_create(hf_pf_over_current_stack, sizeof(hf_pf_over_current_stack),
				HF_OVER_CURRENT_PRIO, THREAD_CREATE_STACKTEST,
				hf_pf_over_current_event_service, NULL, "hf pf over current");
	}
   return hf_over_current_pid;
}

