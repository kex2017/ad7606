#include <errno.h>

#include "kldaq.h"
#include "xtimer.h"
#include "log.h"

int kldaq_init(kldaq_dev_t *daq)
{
   int ret;
   if (!daq || !daq->cnf || !daq->driver) {
      return -ENODEV;
   }
   if (daq->cnf->init && daq->driver->init) {
      if ((ret = daq->cnf->init(daq->cnf)) != 0) {
         printf("DAQ Config Init Failed");
         return ret;
      }

      if ((ret = daq->cnf->download_image(daq->cnf)) != 0) {
         printf("DAQ Download Image Failed");
         return ret;
      }

      if ((ret = daq->driver->init(daq)) != 0) {
         printf("DAQ Function Init Failed");
         return ret;
      }
   }
   return -ENOTSUP;
}


int kldaq_download_image(kldaq_dev_t *daq)
{
   if (!daq || !daq->cnf) {
       return -ENODEV;
   }

   if (daq->cnf->download_image) {
       return daq->cnf->download_image(daq->cnf);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_is_microcode_work_ok(kldaq_dev_t *daq)
{
   if (!daq || !daq->cnf) {
       return -ENODEV;
   }

   if (daq->cnf->is_microcode_work_ok) {
       return daq->cnf->is_microcode_work_ok(daq->cnf);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_power(kldaq_dev_t *daq, enum klfpga_power_state power)
{
    if (!daq || !daq->cnf) {
        return -ENODEV;
    }

    if (daq->cnf->power) {
        return daq->cnf->power(daq->cnf, power);
    }
    else {
        return -ENOTSUP;
    }
}

int kldaq_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->trigger_single_sample) {
       return daq->driver->trigger_single_sample(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_clear_sample_flag(kldaq_dev_t *daq, uint8_t channels)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->clear_sample_flag) {
       return daq->driver->clear_sample_flag(daq, channels);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_trigger_multi_channels_sample(kldaq_dev_t *daq, uint8_t* channels, size_t count)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->trigger_multi_channels_sample) {
       return daq->driver->trigger_multi_channels_sample(daq, channels, count);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_is_sample_done(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->is_sample_done) {
       return daq->driver->is_sample_done(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_enable_order_read(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->is_sample_done) {
       return daq->driver->enable_order_read(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_read_cycle_num(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->read_cycle_num) {
       return daq->driver->read_cycle_num(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_write_cycle_num(kldaq_dev_t *daq, uint8_t channel, uint16_t cycle_num)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->write_cycle_num) {
       return daq->driver->write_cycle_num(daq, channel, cycle_num);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_stop_sample(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->stop_sample) {
       return daq->driver->stop_sample(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_enable_entire_read(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->enable_entire_read) {
       return daq->driver->enable_entire_read(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_read_entire_data(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->read_entire_data) {
       return daq->driver->read_entire_data(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

uint32_t kldaq_get_length(kldaq_dev_t *daq, uint8_t channel)
{
    if (!daq || !daq->driver) {
        return -ENODEV;
    }

    if (daq->driver->get_length) {
        return daq->driver->get_length(daq, channel);
    }
    else {
        return -ENOTSUP;
    }
}

uint32_t kldaq_get_pd_ns(kldaq_dev_t *daq, uint8_t channel)
{
    if (!daq || !daq->driver) {
        return -ENODEV;
    }

    if (daq->driver->get_pd_ns) {
        return daq->driver->get_pd_ns(daq, channel);
    }
    else {
        return -ENOTSUP;
    }
}

int kldaq_read_data(kldaq_dev_t *daq, uint8_t channel, void *dest,uint32_t addr,size_t count)
{
    if (!daq || !daq->driver) {
        return -ENODEV;
    }

    if (daq->driver->read_data) {
        return daq->driver->read_data(daq, channel, dest,addr, count);
    }
    else {
        return -ENOTSUP;
    }
}


uint16_t kldaq_get_max(kldaq_dev_t *daq, uint8_t channel)
{
    if (!daq || !daq->driver) {
        return -ENODEV;
    }

    if (daq->driver->get_max) {
        return daq->driver->get_max(daq, channel);
    }
    else {
        return -ENOTSUP;
    }
}

int kldaq_is_over_threshold(kldaq_dev_t *daq, uint8_t channel)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->is_over_threshold) {
       return daq->driver->is_over_threshold(daq, channel);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->set_threshold) {
       return daq->driver->set_threshold(daq, channel, threshold);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_set_changerate(kldaq_dev_t *daq, uint8_t channel, uint16_t changerate)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->set_changerate) {
       return daq->driver->set_changerate(daq, channel, changerate);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_reset_pd_flag(kldaq_dev_t *daq, uint16_t channels)
{
   if (!daq || !daq->driver) {
       return -ENODEV;
   }

   if (daq->driver->reset_pd_flag) {
       return daq->driver->reset_pd_flag(daq, channels);
   }
   else {
       return -ENOTSUP;
   }
}

int kldaq_set_channel_pd_cfg(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold, uint16_t change_rate)
{
	if (!daq || !daq->driver) {
		return -ENODEV;
	}

	if (daq->driver->set_channel_pd_cfg) {
		return daq->driver->set_channel_pd_cfg(daq, channel, threshold, change_rate);
	} else {
		return -ENOTSUP;
	}
}

int kldaq_read_fpga_version(kldaq_dev_t *daq, uint16_t *fpga_version)
{
    if (!daq || !daq->driver) {
        return -ENODEV;
    }

    if (daq->driver->read_version) {
        return daq->driver->read_version(daq, fpga_version);
    } else {
        return -ENOTSUP;
    }
}

int kldaq_set_pd_work_mode(kldaq_dev_t *daq, uint8_t mode)
{
    if (!daq || !daq->driver) {
        return -ENODEV;
    }

    if (daq->driver->set_pd_work_mode) {
        return daq->driver->set_pd_work_mode(daq, mode);
    } else {
        return -ENOTSUP;
    }
}

int kldaq_write_reg(kldaq_dev_t *daq, uint8_t channel, uint16_t reg, uint16_t value)
{
	if (!daq || !daq->driver) {
		return -ENODEV;
	}

	if (daq->driver->write_reg) {
		return daq->driver->write_reg(daq, channel, reg, value);
	} else {
		return -ENOTSUP;
	}
}

int kldaq_set_fpga_time(kldaq_dev_t *daq,uint32_t time)
{
   if (!daq || !daq->driver) {
      return -ENODEV;
   }
   if (daq->driver->set_fpga_time) {
      return daq->driver->set_fpga_time(daq, time);
   } else {
      return -ENOTSUP;
   }
}

int kldaq_get_pd_curve_time(kldaq_dev_t *daq,uint32_t *time)
{
   if (!daq || !daq->driver) {
      return -ENODEV;
   }
   if (daq->driver->get_pd_curve_time) {
      return daq->driver->get_pd_curve_time(daq, time);
   } else {
      return -ENOTSUP;
   }
}

uint16_t kldaq_read_reg(kldaq_dev_t *daq, uint8_t channel, uint16_t reg)
{
	if (!daq || !daq->driver) {
		return -ENODEV;
	}
	if (daq->driver->read_reg) {
		return daq->driver->read_reg(daq, channel, reg);
	} else {
		return -ENOTSUP;
	}
}


#define BUF_SIZE 20480
static uint8_t fpga_data_buf[BUF_SIZE] __attribute__((section(".big_data")));
int kldaq_test_all_channels(kldaq_dev_t *daq)
{
   (void)daq;
   unsigned int i;
   unsigned int chax_dat_len;

   uint8_t channels[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
   kldaq_trigger_multi_channels_sample(daq, channels, sizeof(channels));

//   launch_pulse_signal(1);
   xtimer_ticks32_t start = xtimer_now();
   xtimer_ticks32_t end, diff;

   int retry_time = 10;
   for (i = 0; i < 10; i++) {
      while (!kldaq_is_sample_done(daq, i) && retry_time > 0){ //等待本通道(通道0~9中的一个)采集完毕
         xtimer_usleep(500000);//实测700ms
         retry_time--;
      }
      if (retry_time == 0) {
         printf("FPGA not Done yet!!!\n");
         return -1;
      }
      end = xtimer_now();
      diff = xtimer_diff(end, start);
      chax_dat_len = kldaq_get_length(daq, i); //读取本通道的数据长度 //这里的数据长度标的是byte长度，而每次从fpga读是读一个16bit无符号数
      printf("channel %d: chax_dat_len=%d; interval is %"PRIu32" \r\n", i, chax_dat_len/2, diff.ticks32);
      chax_dat_len = kldaq_read_data(daq, i, fpga_data_buf,0, chax_dat_len);
//      LOG_DUMP((const char*)fpga_data_buf, 10);

   }
   return 0;
}
