#ifndef SRC_SWITCH_VALUE_H_
#define SRC_SWITCH_VALUE_H_


#define SW_1           GPIO_PIN(PORT_B,3)//前左
#define SW_2           GPIO_PIN(PORT_B,6)//前右
#define SW_3           GPIO_PIN(PORT_B,4)//后左
#define SW_4           GPIO_PIN(PORT_B,7)//后右
#define SW_5           GPIO_PIN(PORT_B,9)//震动防盗

void switch_init(void);

uint8_t read_sw(uint8_t sw_no);


#endif /* SRC_SWITCH_VALUE_H_ */
