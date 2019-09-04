#include "low_power.h"
#include "log.h"
#include "daq.h"
#include "x_delay.h"
#include "periph/stm32f10x_std_periph.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "board.h"
#include "period_data.h"
#include "upgrade_from_flash.h"
/*

# 低功耗模式功能
1.关闭FPGA电源和模拟部分电源

2. 7.5V进入，8v退出
*/

/****************************************************************/
static pwr_status_t g_pwr_status = TURN_ON;

pwr_status_t get_pwr_status(void)
{
    return g_pwr_status;
}

void set_pwr_status(pwr_status_t pwr_status)
{
    g_pwr_status = pwr_status;
}

void power_on_all(void)
{
    ext_pm_power_on_all();
    if(get_pwr_status() == TURN_OFF){
        download_fpga_image(FPGA_A_CS);
        delay_s(1);
        download_fpga_image(FPGA_B_CS);
        delay_s(1);
        download_fpga_image(FPGA_C_CS);
        delay_s(5);
    }
    set_pwr_status(TURN_ON);
}

void power_off_all(void)
{
    ext_pm_power_off_fpga();
    set_pwr_status(TURN_OFF);
}

/****************************************************************/

void display_battery_voltage(void)
{
    LOG_INFO("Get battery voltage as %f", get_bat_vol());
}

uint8_t get_battery_mode(void)
{
    uint8_t mode = 0;
    float bat_vol = get_bat_vol();
    display_battery_voltage();
    if (bat_vol < LOW_POWER_VOL) {//低功耗模式
        mode = LOW_PW_MODE;
    }
    else if(bat_vol > NORMAL_VOL){//正常模式
        mode = NORMAL_MODE;
    }
    else{
        mode = TRANSITION_MODE; //过渡模式:保持当前状态
    }

    return mode;
}

static uint8_t enter_low_pw = 0;

void set_low_pw(void)
{
    enter_low_pw = 1;
}

void clear_low_pw(void)
{
    enter_low_pw = 0;
}

uint8_t get_low_pw(void)
{
    return enter_low_pw;
}

void change_pw_mode_by_bat_level(void)
{
    switch (get_battery_mode()) {
    case NORMAL_MODE:
        LOG_INFO("now pw mode is :NORMAL_MODE, power all on ...");
        power_on_all();
        clear_low_pw();
        break;
    case LOW_PW_MODE:
        LOG_INFO("now pw mode is :LOW_PW_MODE, power all off ...");
        power_off_all();
        set_low_pw();
        break;
    case TRANSITION_MODE:
        if(TURN_OFF == get_pwr_status())
            LOG_INFO("now pw mode is :TRANSITION_MODE, power all off, charging ...");//保持当前开关状态
        else
            LOG_INFO("now pw mode is :TRANSITION_MODE, power all on ,It's about to go into low power mode...");
        break;
    default:
        break;
    }
}

void test_get_bat_vol(void)
{
    battery_voltage_init();

    display_battery_voltage();
}



