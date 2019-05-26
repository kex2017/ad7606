#include "kldaq_fpga_fsmc.h"

#include "log.h"
#include "klfpga.h"
#include "board.h"
#include "xtimer.h"


static int kldaq_fpga_fsmc_init(kldaq_dev_t *daq);
static int kldaq_fpga_fsmc_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel);
static int kldaq_fpga_fsmc_trigger_multi_channels_sample(kldaq_dev_t *daq, uint8_t *channels, size_t count);
static int kldaq_fpga_fsmc_is_sample_done(kldaq_dev_t *daq, unsigned char channel);

static uint32_t kldaq_fpga_fsmc_get_length(kldaq_dev_t *daq, uint8_t channel);
static uint32_t kldaq_fpga_fsmc_get_pd_ns(kldaq_dev_t *daq, uint8_t channel);
static int kldaq_fpga_fsmc_read_data(kldaq_dev_t *daq, uint8_t channel, void *dest,uint32_t addr, size_t size);
static uint16_t kldaq_fpga_fsmc_get_max(kldaq_dev_t *daq, uint8_t channel);

static int kldaq_fpga_fsmc_is_over_threshold(kldaq_dev_t *daq, uint8_t channel);
static int kldaq_fpga_fsmc_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold);
static int kldaq_fpga_fsmc_reset_pd_flag(kldaq_dev_t *daq, uint16_t channels);
static int kldaq_fpga_fsmc_set_channel_pd_cfg(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold, uint16_t change_rate);
static int kldaq_fpga_fsmc_set_time(kldaq_dev_t *daq,uint32_t time);
static int kldaq_fpga_fsmc_get_time(kldaq_dev_t *daq,uint32_t *time);
static int kldaq_fpga_fsmc_write_reg(kldaq_dev_t *daq, uint16_t reg, uint16_t value);
static uint16_t kldaq_fpga_fsmc_read_reg(kldaq_dev_t *daq, uint16_t reg);
static int kldaq_fpga_fsmc_read_version(kldaq_dev_t *daq, unsigned short *fpga_version);
static int kldaq_fpga_fsmc_set_fpga_work_mode(kldaq_dev_t *daq, uint8_t mode);
const kldaq_func_t kldaq_fpga_fsmc_driver = {
    .init = kldaq_fpga_fsmc_init,
    .trigger_single_sample = kldaq_fpga_fsmc_trigger_one_sample,
    .trigger_multi_channels_sample = kldaq_fpga_fsmc_trigger_multi_channels_sample,
    .is_sample_done = kldaq_fpga_fsmc_is_sample_done,
    .get_length = kldaq_fpga_fsmc_get_length,
	.get_pd_ns = kldaq_fpga_fsmc_get_pd_ns,
    .read_data = kldaq_fpga_fsmc_read_data,
    .get_max = kldaq_fpga_fsmc_get_max,
    .is_over_threshold = kldaq_fpga_fsmc_is_over_threshold,
    .set_threshold = kldaq_fpga_fsmc_set_threshold,
	.reset_pd_flag = kldaq_fpga_fsmc_reset_pd_flag,
	.set_channel_pd_cfg = kldaq_fpga_fsmc_set_channel_pd_cfg,
	.set_fpga_time = kldaq_fpga_fsmc_set_time,
	.get_pd_curve_time = kldaq_fpga_fsmc_get_time,
	.write_reg = kldaq_fpga_fsmc_write_reg,
	.read_reg = kldaq_fpga_fsmc_read_reg,
	.read_version = kldaq_fpga_fsmc_read_version,
	.set_pd_work_mode = kldaq_fpga_fsmc_set_fpga_work_mode,


};

kldaq_dev_t fpga_fsmc_dev_without_img_download = {&kldaq_fpga_fsmc_driver, &kldaq_fpga_dummy_cnf_ops};
kldaq_dev_t fpga_fsmc_dev_with_img_download = {&kldaq_fpga_fsmc_driver, &kldaq_fpga_ps_cnf_ops};

#ifndef FPGA_BASE_ADDR
#error "The FPGA_BASE_ADDR of fsmc_fpga_daq IS Not Defined"
#endif

uint16_t  fpga_read(uint32_t offset);
void  fpga_write(uint32_t offset, uint16_t data);

static int kldaq_fpga_fsmc_init(kldaq_dev_t *daq)
{
   (void)daq;
   LOG_INFO("FPGA FSMC DAQ init...Nothing to do");
   return 0;
}

static int kldaq_fpga_fsmc_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;
   (void)channel;
   uint16_t reg0 = (REG0_CHANNEL_STARTBIT << channel);
   fpga_write(0, reg0);
   return 0;
}

//可同时触发多个通道开始采集
static int kldaq_fpga_fsmc_trigger_multi_channels_sample(kldaq_dev_t *daq, uint8_t *channels, size_t count)
{
   (void)daq;
   uint16_t reg = 0;
   for (size_t i = 0; i < count; i++) {
      reg |= (REG0_CHANNEL_STARTBIT << channels[i]);
   }
   printf("The register0 value: 0x%4X\r\n", reg);
   fpga_write(0, reg);
   return 0;
}

//return 1 已采集完毕   0:还在采集
static int kldaq_fpga_fsmc_is_sample_done(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;
   int flag = 0;
   volatile unsigned short dat;

   fpga_write(0, channel); //选择对应的通道
   dat = fpga_read(1); //读取对应通道是否采集完毕
   flag = dat & CHA_STA_REG_OVER_MASK;
//   LOG_DEBUG("Check fsmc fpga channel(%d) sample done flag:%d",channel, flag);
   return flag;
}

//返回的是byte的长度, 但从fpga读取时是按照short读取的, 两者的长度注意区分
static uint32_t kldaq_fpga_fsmc_get_length(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;

   unsigned int dat_len;
   unsigned short tmp_h;
   unsigned short tmp_l;

   fpga_write(0, channel);

   tmp_h = fpga_read(2);
   tmp_l = fpga_read(3);

   dat_len = (tmp_h << 16) + tmp_l;

   return (dat_len << 1); //返回的数据长度是uint8_t的长度*/

}

static uint32_t kldaq_fpga_fsmc_get_pd_ns(kldaq_dev_t *daq, uint8_t channel)
{
	(void) daq;

	float tick_count;
	float count_per_sec;
	unsigned short tmp_h;
	unsigned short tmp_l;

	fpga_write(0, channel);

	tmp_h = fpga_read(8);
	tmp_l = fpga_read(9);
	tick_count = (float)((tmp_h << 16) + tmp_l);

	tmp_h = fpga_read(10);
	tmp_l = fpga_read(11);

	count_per_sec = (float)(tmp_h << 16) + tmp_l;

	return (uint32_t)(tick_count * 1000000000 / count_per_sec) ;
}
void delay_sram(unsigned int i)
{
	unsigned char m;
	for(;i>0;i--)
		for(m=0;m<20;m++);
}

void close_prpd_produce(void)
{
    unsigned short reg15;

    reg15 = fpga_read(15);
    reg15 &= 0xfffd;
    fpga_write(15, reg15);
}

void open_prpd_produce(void)
{
    unsigned short reg15;

    reg15 = fpga_read(15);
    reg15 |= 0x2;
    fpga_write(15, reg15);
}

static int kldaq_fpga_fsmc_read_data(kldaq_dev_t *daq, uint8_t channel, void *data, uint32_t addr, size_t byte_len)
{
   (void)daq;
   (void)addr;

   unsigned int index = 0;
   volatile unsigned short m;// max_value = 0;
   unsigned int short_len = byte_len / 2;
   volatile unsigned short *tmp = (volatile unsigned short *)data;
   uint32_t sum = 0;

   if( channel > 12 && channel < 16)
   {
       close_prpd_produce();
   }
   fpga_write(0, (1 << 14) | channel);

//   m = fpga_read(7);
   m = fpga_read(7);    //TODO 谢工在fpga里面多给了我们读取了1个short型的数据，但是给我们的每个通道长度没有变，这个问题还有待解决
   m++;

   for (index = 0; index < short_len; index++) {
	   m = fpga_read(7);
	   //除了工频电流(4-9通道)的数据是16bit外, 过压、局放、高频电流的数据都是12bit, 也就是不会出现负数
	   //max_vaule只为计算高频电流值
//	   max_value = max_value > m ? max_value : m;
	   sum += m;
	   //TODO: try to more Precise for delay
       delay_sram(10000);
       *tmp = m;
       tmp++;
   }

   // 读取完成后再打开图谱产生功能
   if( channel > 12 && channel < 16)
   {
       open_prpd_produce();
   }

   return sum / short_len;
}


static uint16_t kldaq_fpga_fsmc_get_max(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;

   volatile unsigned short max;
   volatile unsigned short min;
   uint16_t tmp = 0;

   fpga_write(0, channel);

   max = fpga_read(4);
   min = fpga_read(5);

   if ((max - 2048) >= (2048 - min)) {
       tmp = max;
   }
   else {
       tmp = min;
   }

   return tmp;
}

static int kldaq_fpga_fsmc_is_over_threshold(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;
   unsigned short ret = 0;
   fpga_write(0, channel);
   ret = fpga_read(1);
   return ret & CHA_STA_REG_INT_MASK;
}

static int kldaq_fpga_fsmc_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold)
{
   (void)daq;
   fpga_write(0, channel);
   fpga_write(1, threshold);
   return 0;
}

static int kldaq_fpga_fsmc_reset_pd_flag(kldaq_dev_t *daq, uint16_t channels)
{
	(void) daq;
	fpga_write(7, channels);
	return 0;
}

static int kldaq_fpga_fsmc_set_channel_pd_cfg(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold, uint16_t change_rate)
{
	(void) daq;
	fpga_write(0, channel);
	fpga_write(1, threshold);
	fpga_write(6, change_rate);
	return 0;
}


static int kldaq_fpga_fsmc_set_time(kldaq_dev_t *daq,uint32_t time)
{
   (void) daq;
   fpga_write(12,(time>>16)&0xffff );//高16bit写到reg12
   fpga_write(13,time&0xffff );//低16bit写到reg12

//   uint32_t test_time;
//   test_time = (fpga_read(12)<<16 | fpga_read(13));
//   printf("read from fpga time:%ld ok!\r\n",test_time);

   return 0;
}

static int kldaq_fpga_fsmc_get_time(kldaq_dev_t *daq,uint32_t *time)
{
   (void) daq;

   *time = (fpga_read(12)<<16 | fpga_read(13));

   return 0;
}

//STM32通过写Reg15的Bit0来控制设置FPGA产生局放原始波形策略，
//注意要采用读，修改，写的方式，因为Reg15的其它Bit还有其它信息
#define CAPTURE_MODE 0
#define COLLECT_MODE 1
static int kldaq_fpga_fsmc_set_fpga_work_mode(kldaq_dev_t *daq, uint8_t mode)
{
    (void)daq;
    unsigned short reg15;
    //设置FPGA工作在捕获模式，捕获到越限或变化率越限保存原始波形。
    if (mode == CAPTURE_MODE) {
        reg15 = fpga_read(15);
        reg15 &= 0xfffe;
        fpga_write(15, reg15);
    }
    //设置 FPGA工作在采集模式，FPGA在收到采集命令(reg8的bit0为1)则无条件采集一段局放原始波形。
    else if (mode == COLLECT_MODE) {
        reg15 = fpga_read(15);
        reg15 |= 1;
        fpga_write(15, reg15);
    }
    return 0;
}

static int kldaq_fpga_fsmc_read_version(kldaq_dev_t *daq, unsigned short *fpga_version)
{
    (void)daq;
    for (int i = 0; i < 32; i++) {
        fpga_version[i] = fpga_read(14);
    }
    return 0;
}



void  fpga_write(uint32_t offset, uint16_t data)
{
   *((volatile unsigned short*)(FPGA_BASE_ADDR + (uint32_t)((uint32_t)offset << 17))) = (unsigned short)(data);
}
uint16_t  fpga_read(uint32_t offset)
{
   return *((volatile unsigned short*)(FPGA_BASE_ADDR + (uint32_t)((uint32_t)offset << 17)));
}

static int kldaq_fpga_fsmc_write_reg(kldaq_dev_t *daq, uint16_t reg, uint16_t value)
{
	(void) daq;
	fpga_write(reg, value);
	return 0;
}

static uint16_t kldaq_fpga_fsmc_read_reg(kldaq_dev_t *daq, uint16_t reg)
{
	uint16_t ret = 0;

	(void) daq;
	ret = fpga_read(reg);
	return ret;
}

void fpga_set_over_voltage_threshold(uint16_t *threshold)
{
    uint8_t channel = 0;
    for (channel = 0; channel < 4; channel++) {
        fpga_write(0, channel);
        fpga_write(1, threshold[channel]);
    }
}

//可同时触发多个通道开始采集
void fpga_start_run(unsigned short channels_mask)
{
    fpga_write(0, channels_mask);
}


void daq_test_fsmc_main(void)
{
   kldaq_test_all_channels(&fpga_fsmc_dev_without_img_download);
}

