#include "data_processor.h"
#include "thread.h"
#include "msg.h"
#include "timex.h"
#include "xtimer.h"
#include "frame_common.h"
#include "periph/rtt.h"
#include "daq.h"
#include "frame_encode.h"
#include "frame_handler.h"
#include "periph/gpio.h"


static void delay_ms(int ms)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}

#define  FPGA_INT1         GPIO_PIN(PORT_E, 2)

kernel_pid_t _data_processor_pid;
void on_int(void *arg)
{
    (void)arg;

    msg_t msg;
    printf("int ............\r\n");
    msg_send(&msg, _data_processor_pid);
}

void set_default_threshold_rate(void)
{
    for(uint8_t i = 0; i < HF_CHAN_NUM; i++){
        daq_spi_set_threshold(i, DEFAULT_THRESHOLD);
        daq_spi_set_change_rate(i, DEFAULT_CHANGERATE);
    }
}

uint8_t g_curve_buf[CURVE_LEN] = {0};
uint32_t g_chan_cnt[HF_CHAN_NUM] = {0};
void *data_processor(void *arg)
{
    (void)arg;
    msg_t recv_msg;
    uint16_t data_len = 0;

    gpio_init_int(FPGA_INT1, GPIO_IN, GPIO_FALLING, on_int, NULL);
    set_default_threshold_rate();
    while (1) {
        msg_receive(&recv_msg);
        for(uint8_t channel = 0; channel < HF_CHAN_NUM; channel++){
            if(daq_spi_sample_done_check(channel)){
                data_len = daq_spi_get_data_len(channel);
                for(uint16_t len = 0; len < data_len; len++){
                    daq_spi_sample_data_read(channel, g_curve_buf, 0, data_len);
                    g_chan_cnt[channel] = daq_spi_chan_cnt_since_plus(channel);
                    LOG_INFO("read chan %d cnt is %d", channel, g_chan_cnt[channel]);
                    daq_spi_clear_data_done_flag(channel);
                }
            }
        }
        delay_ms(2000);
    }
    return NULL;
}

#define DATA_PROCESSOR_PRIORITY (6)
static char data_processor_thread_stack[THREAD_STACKSIZE_MAIN*2];
void data_processor_thread_init(void)
{
    LOG_INFO("Data processor thread start....\r\n");
    _data_processor_pid = thread_create(data_processor_thread_stack, sizeof(data_processor_thread_stack),
                                      DATA_PROCESSOR_PRIORITY,
                                      THREAD_CREATE_STACKTEST, data_processor, NULL, "data_process");
//    								do_receive_pid_hook(_pid);

}


