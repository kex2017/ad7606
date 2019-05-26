#include "../kl-stm32-cg/include/board.h"
#include "periph/gpio.h"
#include "xtimer.h"


/* power managerment switch pins */
//#define  ADC_SW_PIN        GPIO_PIN(PORT_F,6)
//#define  AMP_SW_PIN        GPIO_PIN(PORT_F,7)
//#define  FPGA_SW_PIN       GPIO_PIN(PORT_F,8)
#define  GPS_SW_PIN        GPIO_PIN(PORT_F,9)
#define  V5V1_SW_PIN       GPIO_PIN(PORT_E,5)
#define  V5V2_SW_PIN       GPIO_PIN(PORT_E,6)
#define  JF_SW_PIN         GPIO_PIN(PORT_F,6)
#define  GY_SW_PIN         GPIO_PIN(PORT_F,8)

#define  GPS_RESET_PIN     GPIO_PIN(PORT_C,4)
#define  GPS_SLEEP_PIN     GPIO_PIN(PORT_C,7)

void ext_pm_ctrl_init(void)
{
   gpio_init(GPS_SW_PIN, GPIO_OUT);
   gpio_init(GPS_RESET_PIN, GPIO_OUT);
   gpio_init(GPS_SLEEP_PIN, GPIO_OUT);
   gpio_init(V5V1_SW_PIN, GPIO_OUT);
   gpio_init(V5V2_SW_PIN, GPIO_OUT);
   gpio_init(JF_SW_PIN, GPIO_OUT);
   gpio_init(GY_SW_PIN, GPIO_OUT);
}

void ext_pm_power_on_all(void)
{
   ext_pm_power_on_gps();
   ext_pm_power_on_gy();

   reset_ext_fpga_power();
}

void reset_ext_fpga_power(void)
{
	ext_pm_power_off_jf(); // add by Xiewei, power off 1 seconds, and then power on
	xtimer_sleep(1);
	ext_pm_power_on_jf();
}

void ext_pm_power_on_gps(void)
{
   gpio_clear(GPS_SW_PIN);
   gpio_set(GPS_RESET_PIN);
   gpio_set(GPS_SLEEP_PIN); //禁止4G模块休眠
}

void ext_pm_power_off_gps(void)
{

   gpio_set(GPS_SW_PIN);

}

void ext_pm_power_on_jf(void)
{
   gpio_clear(JF_SW_PIN);
}

void ext_pm_power_off_jf(void)
{

   gpio_set(JF_SW_PIN);
}

void ext_pm_power_on_gy(void)
{
   gpio_clear(GY_SW_PIN);
}

void ext_pm_power_off_gy(void)
{

   gpio_set(GY_SW_PIN);
}
