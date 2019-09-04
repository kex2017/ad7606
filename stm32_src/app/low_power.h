#ifndef APP_LOW_POWER_H_
#define APP_LOW_POWER_H_
#include <stdint.h>

#define LOW_POWER_VOL 7.5
#define NORMAL_VOL 8.0

enum{
    NORMAL_MODE,
    LOW_PW_MODE,
    TRANSITION_MODE,
};

typedef enum{
    TURN_OFF,
    TURN_ON
}pwr_status_t;

void power_on_all(void);
void power_off_all(void);
uint8_t get_low_pw(void);
pwr_status_t get_pwr_status(void);

void battery_voltage_init(void);
void change_pw_mode_by_bat_level(void);
void display_battery_voltage(void);
void change_pw_mode_by_bat_level(void);

#endif /* APP_LOW_POWER_H_ */
