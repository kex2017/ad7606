
#ifndef KLDAQ_H_
#define KLDAQ_H_

#include <stdint.h>
#include "klfpga.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kldaq_fun kldaq_func_t;


typedef struct {
    const kldaq_func_t *driver;  /**< DAQ driver */
    const klfpga_conf_t *cnf; /*device management element*/
} kldaq_dev_t;

struct kldaq_fun {
   int (*init)(kldaq_dev_t *daq);

   int (*trigger_single_sample)(kldaq_dev_t *daq, uint8_t channel);

   int (*trigger_multi_channels_sample)(kldaq_dev_t *daq, uint8_t *channels, size_t count);

   int (*is_sample_done)(kldaq_dev_t *daq, uint8_t channel);

   int (*clear_sample_flag)(kldaq_dev_t *daq, uint8_t channels);

   int (*enable_order_read)(kldaq_dev_t *daq, uint8_t channels);

   int (*read_cycle_num)(kldaq_dev_t *daq, uint8_t channels);

   int (*write_cycle_num)(kldaq_dev_t *daq, uint8_t channels, uint16_t cycle_num);

   int (*stop_sample)(kldaq_dev_t *daq, uint8_t channels);

   int (*enable_entire_read)(kldaq_dev_t *daq, uint8_t channels);

   int (*read_entire_data)(kldaq_dev_t *daq, uint8_t channels);

   uint32_t (*get_length)(kldaq_dev_t *daq, uint8_t channel);

   uint32_t (*get_pd_ns)(kldaq_dev_t *daq, uint8_t channel);

   int (*read_data)(kldaq_dev_t *daq, uint8_t channel, void *buff, uint32_t addr, size_t size);

   uint16_t (*get_max)(kldaq_dev_t *daq, uint8_t channel);

   int (*is_over_threshold)(kldaq_dev_t *daq, uint8_t channel);

   int (*set_threshold)(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold);

   int (*set_changerate)(kldaq_dev_t *daq, uint8_t channel, uint16_t changerate);

   int (*reset_pd_flag)(kldaq_dev_t *daq, uint16_t channels);

   int (*set_channel_pd_cfg)(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold, uint16_t change_rate);

   int (*write_reg)(kldaq_dev_t *daq, uint8_t channel, uint16_t reg, uint16_t value);

   int (*set_fpga_time)(kldaq_dev_t * daq,uint32_t time);

   int (*get_pd_curve_time)(kldaq_dev_t * daq,uint32_t *time);

   uint16_t (*read_reg)(kldaq_dev_t *daq, uint8_t channel, uint16_t reg);

   int (*read_version)(kldaq_dev_t *daq, uint16_t *fpga_version);

   int (*set_pd_work_mode)(kldaq_dev_t *daq, uint8_t mode);

};


int kldaq_init(kldaq_dev_t *daq);
int kldaq_download_image(kldaq_dev_t *daq);
int kldaq_is_microcode_work_ok(kldaq_dev_t *daq);
int kldaq_power(kldaq_dev_t *daq, enum klfpga_power_state power);


uint16_t kldaq_read_reg(kldaq_dev_t *daq, uint8_t channel, uint16_t reg);
int kldaq_write_reg(kldaq_dev_t *daq, uint8_t channel, uint16_t reg, uint16_t value);

int kldaq_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel);
int kldaq_trigger_multi_channels_sample(kldaq_dev_t *daq, uint8_t* channels, size_t count);
int kldaq_is_sample_done(kldaq_dev_t *daq, uint8_t channel);
int kldaq_clear_sample_flag(kldaq_dev_t *daq, uint8_t channels);
int kldaq_enable_order_read(kldaq_dev_t *daq, uint8_t channel);
int kldaq_read_cycle_num(kldaq_dev_t *daq, uint8_t channel);
int kldaq_write_cycle_num(kldaq_dev_t *daq, uint8_t channel, uint16_t cycle_num);
int kldaq_stop_sample(kldaq_dev_t *daq, uint8_t channel);
int kldaq_enable_entire_read(kldaq_dev_t *daq, uint8_t channel);
int kldaq_read_entire_data(kldaq_dev_t *daq, uint8_t channel);

uint32_t kldaq_get_length(kldaq_dev_t *daq, uint8_t channel);
uint32_t kldaq_get_pd_ns(kldaq_dev_t *daq, uint8_t channel);
int kldaq_read_data(kldaq_dev_t *daq, uint8_t channel, void *dest,uint32_t addr, size_t count);
uint16_t kldaq_get_max(kldaq_dev_t *daq, uint8_t channel);

int kldaq_is_over_threshold(kldaq_dev_t *daq, uint8_t channel);
int kldaq_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold);
int kldaq_set_changerate(kldaq_dev_t *daq, uint8_t channel, uint16_t changerate);
int kldaq_reset_pd_flag(kldaq_dev_t *daq, uint16_t channels);
int kldaq_set_channel_pd_cfg(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold, uint16_t change_rate);
int kldaq_read_fpga_version(kldaq_dev_t *daq, uint16_t *fpga_version);
int kldaq_set_pd_work_mode(kldaq_dev_t *daq, uint8_t mode);

int kldaq_set_fpga_time(kldaq_dev_t *daq,uint32_t time);
int kldaq_get_pd_curve_time(kldaq_dev_t *daq,uint32_t *time);


int kldaq_test_all_channels(kldaq_dev_t *daq);

#define REG0_CHANNEL_STARTBIT 0x0010
#define CHA_STA_REG_OVER_MASK 0x0001//表示上次ARM要求采集的数据是否采集完毕，新开启一次采集自动清除
#define CHA_STA_REG_INT_MASK 0x0002//表示该通道是否处于中断状态，由ARM在中断服务程序中写寄存器清除
#define CHA_STA_REG_CYCLE_NUM_MASK 0x000F

#ifdef __cplusplus
}
#endif

#endif /* KLDAQ_H_ */
