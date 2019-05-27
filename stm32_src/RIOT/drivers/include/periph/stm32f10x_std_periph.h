#ifndef STM32F10X_STD_PERIPH_H
#define STM32F10X_STD_PERIPH_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

//adc1 dma1 tim4 sample
/*	采样计算例子
 *  系统时钟为72MHZ,FPCLK2=72 000 000HZ
 *  对50HZ交流信号进行采集，一个周期为20MS, 采集8路信号，每个信号采用64次，
 *  20*1000us/64=312us,即每隔312us要启动一次采集信号（需要采集8路信号），采集64次后正好是一个周期；
 *  采集一次8路的时间：（239.5+12.5）*8/9M =224us
 *	224 必须小于 312
 */

int adc1_dma1_tim4_sample_init(uint16_t *channel_list,uint16_t channel_count,uint16_t sample_count,uint16_t sample_duration_ms,uint16_t *sample_buf );
void adc1_dma1_tim4_sample_enable(void);
void adc1_dma1_tim4_sample_disable(void);
typedef void(*adc1_dma1_tim4_sample_done_cb_t)(void);
void set_adc1_dma1_tim4_sample_done_cb(adc1_dma1_tim4_sample_done_cb_t cb);


int adc1_temp_sensor_init(void);
float adc1_temp_sensor_sample(void);

int adc2_sample_init(uint16_t *channel_list,uint16_t channel_count);
uint16_t adc2_sample_by_channel(uint16_t channel);

int adc3_sample_init(uint16_t *channel_list,uint16_t channel_count);
uint16_t adc3_sample_by_channel(uint16_t channel);

//72000000/9000=8000 points per second wave_fre = 9000
int dac1_pa4_dma2_tim6_wave_outopt_init(uint32_t wave_fre);
//wave_start_addr  0x08050000 wave_len
void dac1_pa4_dma2_tim6_wave_outopt(uint32_t wave_start_addr,uint16_t wave_len);

//timeout = prv/40 * rlv (s)
//rlv <= 0xFFF 默认26s
void iwdg_init(void);
void iwdg_feed(void);

void disable_jtag_pins(void);

#ifdef __cplusplus
}
#endif

#endif
