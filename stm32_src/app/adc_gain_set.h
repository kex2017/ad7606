/*
 * adc_gain_set.h
 *
 *  Created on: Mar 28, 2019
 *      Author: xuke
 */

#ifndef ADC_GAIN_SET_H_
#define ADC_GAIN_SET_H_

#include "periph/gpio.h"

#define UNDEF_PIN (GPIO_PIN(PORT_A,0))

#define PFC_A_WR  (GPIO_PIN(PORT_D,12))
#define PFC_A_A0  (GPIO_PIN(PORT_D,14))
#define PFC_A_A1  (GPIO_PIN(PORT_D,13))

#define PFC_B_WR  (GPIO_PIN(PORT_D,9))
#define PFC_B_A0  (GPIO_PIN(PORT_D,11))
#define PFC_B_A1  (GPIO_PIN(PORT_D,10))

#define PFC_C_WR  (GPIO_PIN(PORT_E,14))
#define PFC_C_A0  (GPIO_PIN(PORT_D,8))
#define PFC_C_A1  (GPIO_PIN(PORT_E,15))

#define PFV_A_WR  (GPIO_PIN(PORT_E,11))
#define PFV_A_A0  (GPIO_PIN(PORT_E,13))
#define PFV_A_A1  (GPIO_PIN(PORT_E,12))

#define PFV_B_WR  (GPIO_PIN(PORT_E,8))
#define PFV_B_A0  (GPIO_PIN(PORT_E,10))
#define PFV_B_A1  (GPIO_PIN(PORT_E,9))

#define PFV_C_WR  (GPIO_PIN(PORT_G,0))
#define PFV_C_A0  (GPIO_PIN(PORT_E,7))
#define PFV_C_A1  (GPIO_PIN(PORT_G,1))

#define PFLC_A_WR (GPIO_PIN(PORT_G,7))
#define PFLC_A_A0 (GPIO_PIN(PORT_C,6))
#define PFLC_A_A1 (GPIO_PIN(PORT_G,8))

#define PFLC_B_WR (GPIO_PIN(PORT_G,4))
#define PFLC_B_A0 (GPIO_PIN(PORT_G,6))
#define PFLC_B_A1 (GPIO_PIN(PORT_G,5))

#define PFLC_C_WR (GPIO_PIN(PORT_D,15))
#define PFLC_C_A0 (GPIO_PIN(PORT_G,3))
#define PFLC_C_A1 (GPIO_PIN(PORT_G,2))


#define PFC_TYPE 0
#define PFV_TYPE 1
#define PFLC_TYPE 2

#define PHASE_A 0
#define PHASE_B 1
#define PHASE_C 2


#define GAIN_MULT_1 0
#define GAIN_MULT_10 1
#define GAIN_MULT_100 2
#define GAIN_MULT_1000 3

void adc_gain_pin_init(void);
void set_default_gain_for_all_chan(void);
void set_gain_by_chan_type_and_phase(uint8_t chan_type, uint8_t phase, uint8_t gain_level);


#endif /* ADC_GAIN_SET_H_ */
