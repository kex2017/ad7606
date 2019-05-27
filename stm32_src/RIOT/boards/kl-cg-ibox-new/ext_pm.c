#include "../kl-cg-ibox-new/include/board.h"
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

#define  FPGA_SW1_PIN      GPIO_PIN(PORT_G, 13)
#define  FPGA_SW2_PIN      GPIO_PIN(PORT_G, 14)

void ext_pm_ctrl_init(void)
{
   gpio_init(FPGA_SW1_PIN, GPIO_OUT);
   gpio_init(FPGA_SW2_PIN, GPIO_OUT);
}
void ext_pm_power_on_fpga(void);

void ext_pm_power_on_all(void)
{
//   ext_pm_power_on_gps();
    ext_pm_power_on_fpga();
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

void ext_pm_power_on_fpga(void)
{
   gpio_clear(FPGA_SW1_PIN);
   gpio_clear(FPGA_SW2_PIN);
}

void ext_pm_power_off_fpga(void)
{
   gpio_set(FPGA_SW1_PIN);
   gpio_set(FPGA_SW2_PIN);
}
