#include <stdint.h>

#include "klfpga.h"
#include "board.h"
#include "log.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "timex.h"

#ifndef FPGA_nCFG_PIN
#define  FPGA_nCFG_PIN     (GPIO_PIN(PORT_C,2))
#endif

#ifndef FPGA_DCLK_PIN
#define  FPGA_DCLK_PIN     (GPIO_PIN(PORT_C,3))
#endif

#ifndef FPGA_DATA0_PIN
#define  FPGA_DATA0_PIN    (GPIO_PIN(PORT_F,11))
#endif

#ifndef FPGA_nSTS_PIN
#define  FPGA_nSTS_PIN     (GPIO_PIN(PORT_C,0))
#endif

#ifndef FPGA_DONE_PIN
#define  FPGA_DONE_PIN     (GPIO_PIN(PORT_C,1))
#endif

#ifndef FPGA_PROGRAM_ADDR
#define FPGA_PROGRAM_ADDR 0x08050000
#endif

const uint32_t g_fpga_image_len = 163840;


uint8_t READ_FPGA_CFG_nSTATUS(void)
{
    return (uint8_t)gpio_read(FPGA_nSTS_PIN);
}

uint8_t READ_FPGA_CFG_CONF_DONE(void)
{
    return (uint8_t)gpio_read(FPGA_DONE_PIN);
}

void WRITE_FPGA_CFG_nCONFIG(uint8_t bit_val)
{
    gpio_write(FPGA_nCFG_PIN, bit_val);
}

void WRITE_FPGA_CFG_DCLK(uint8_t bit_val)
{
    gpio_write(FPGA_DCLK_PIN, bit_val);
}

void WRITE_FPGA_CFG_DATA_WITH_HARDWARE_VERSION_101(uint8_t bit_val)
{
    gpio_write(FPGA_DATA0_PIN, bit_val);
}

void fpga_cfg_pins_init(void)
{
    gpio_clear(FPGA_nCFG_PIN);
    gpio_clear(FPGA_DCLK_PIN);
    gpio_clear(FPGA_DATA0_PIN);
    gpio_init(FPGA_nCFG_PIN, GPIO_OUT);
    gpio_init(FPGA_DCLK_PIN, GPIO_OUT);
    gpio_init(FPGA_DATA0_PIN, GPIO_OUT);

    gpio_set(FPGA_nSTS_PIN);
    gpio_set(FPGA_DONE_PIN);
    gpio_init(FPGA_nSTS_PIN, GPIO_IN_PU);
    gpio_init(FPGA_DONE_PIN, GPIO_IN_PU);
}

unsigned char Config_FPGA(unsigned int len, unsigned char *buf, unsigned char first)
{
    unsigned char m;
    unsigned int i, n;

    if (first) {
        LOG_INFO("Fisrt Config: Waiting FPGA prepare to config.");
        xtimer_usleep(60);

        //nCONFIG至少拉低40uS,
        WRITE_FPGA_CFG_nCONFIG(0);
        WRITE_FPGA_CFG_DCLK(0);

        xtimer_usleep(60);
        //V1.1.0---: i = 40;
//        n = 3200;
//        while (--n) {
//            ;         //延时80微秒，以保证cfg_nCONFIG低电平脉冲宽度大于FPGA需求的40us
//        }

        //开始配置
        WRITE_FPGA_CFG_nCONFIG(1);
        while (READ_FPGA_CFG_nSTATUS() == 0) {
            ;  //等待FPGA释放该线nSTATUS拉高了表示已经可以开始配置
        }

        LOG_INFO("FPGA ready to config.");
    }

    for (i = 0; i < len; i++) {
        for (m = 0; m < 8; m++) {
            if (buf[i] & (1 << m)) {
                WRITE_FPGA_CFG_DATA_WITH_HARDWARE_VERSION_101(1);
            }
            else {
                WRITE_FPGA_CFG_DATA_WITH_HARDWARE_VERSION_101(0);
            }
            WRITE_FPGA_CFG_DCLK(1);
            WRITE_FPGA_CFG_DCLK(0);
        }

        if (READ_FPGA_CFG_nSTATUS() == 0) {  //判断有无出错（FLAG_nSTATUS=1的情况下）
            return 1;      //配置出错返回
        }

        if (READ_FPGA_CFG_CONF_DONE()) {     //检测到FPGA配置成功信号
            //unsigned char i;
            n = 20;
            while (--n)    //配置成功后继续送出20个时钟信号
            {
                WRITE_FPGA_CFG_DCLK(1);
                WRITE_FPGA_CFG_DCLK(0);
            }
            LOG_INFO("FPGA Fireware image download OK");
            return 0;      //配置完成
        }
    }

    return 0;
}

unsigned char fpga_img_config(unsigned char first)
{
    return Config_FPGA(g_fpga_image_len, (unsigned char *)FPGA_PROGRAM_ADDR, first);
}
/************************************************
检查FPGA的程序是否丢失，如果返回0，则表示正常。如果
返回1：表示FPGA程序已经丢失，需重新下载FPGA。
************************************************/
unsigned char is_fpga_microcode_work_no_ok(void)
{
    if (READ_FPGA_CFG_CONF_DONE()) {    //如果cfg_done信号为高则表示正常。
        return 0;
    }
    else {
        return 1;
    }
}

/*fpga conf implementation*/

static int kldaq_fpga_dummy_cnf_init(const klfpga_conf_t *daq);
static int kldaq_fpga_dummy_download_image(const klfpga_conf_t *daq);
static int kldaq_fpga_dummy_is_microcode_work_ok(const klfpga_conf_t *daq);
static int kldaq_fpga_dummy_power(const klfpga_conf_t *daq, enum klfpga_power_state power);

static int kldaq_fpga_ps_cnf_init(const klfpga_conf_t *fpga);
static int kldaq_fpga_ps_download_image(const klfpga_conf_t *fpga);
static int kldaq_fpga_ps_is_microcode_work_ok(const klfpga_conf_t *fpga);
static int kldaq_fpga_ps_power(const klfpga_conf_t *fpga, enum klfpga_power_state power);

static int kldaq_fpga_dummy_cnf_init(const klfpga_conf_t *daq)
{
   (void)daq;
   LOG_INFO("DAQ init with DUMMY cnf");

   return 0;
}

static int kldaq_fpga_dummy_download_image(const klfpga_conf_t *daq)
{
   (void)daq;
   LOG_INFO("DAQ image download. with DUMMY cnf");

   return 0;
}

static int kldaq_fpga_dummy_is_microcode_work_ok(const klfpga_conf_t *daq)
{
   (void)daq;
   LOG_INFO("FPGA check microcode. with DUMMY cnf, Always OK!");
   return 1;
}
static int kldaq_fpga_dummy_power(const klfpga_conf_t *daq, enum klfpga_power_state power)
{
   (void)daq;
   (void)power;
   LOG_INFO("DAQ power. with DUMMY cnf");
   return 0;
}


const klfpga_conf_t kldaq_fpga_dummy_cnf_ops = {
    .init = kldaq_fpga_dummy_cnf_init,
    .download_image = kldaq_fpga_dummy_download_image,
    .is_microcode_work_ok = kldaq_fpga_dummy_is_microcode_work_ok,
    .power = kldaq_fpga_dummy_power,
};

const klfpga_conf_t kldaq_fpga_ps_cnf_ops = {
    .init = kldaq_fpga_ps_cnf_init,
    .download_image = kldaq_fpga_ps_download_image,
    .is_microcode_work_ok = kldaq_fpga_ps_is_microcode_work_ok,
    .power = kldaq_fpga_ps_power,
};

static int kldaq_fpga_ps_cnf_init(const klfpga_conf_t *fpga)
{
   (void)fpga;
   fpga_cfg_pins_init();
   return 0;
}
static int kldaq_fpga_ps_download_image(const klfpga_conf_t *fpga)
{
   (void)fpga;
   fpga_img_config(1);
   return 0;
}

static int kldaq_fpga_ps_is_microcode_work_ok(const klfpga_conf_t *fpga)
{
   (void)fpga;
   if (READ_FPGA_CFG_CONF_DONE()) {    //如果cfg_done信号为高则表示正常。
       return 1;
   }
   else {
       return 0;
   }
}
static int kldaq_fpga_ps_power(const klfpga_conf_t *fpga, enum klfpga_power_state power)
{
   (void)fpga;
   (void)power;
   return 0;
}


