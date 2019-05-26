#include "../kl-stm32-cg/include/board.h"
#include "periph/gpio.h"

#define  OVER_VOLTAGE_EVENT_PIN  GPIO_PIN(PORT_E,2)
#define  PARTIAL_DISCHARGE_EVENT_PIN  GPIO_PIN(PORT_G,15)

extern void OVER_VOLTAGE_EXIT2_IRQHandler(void *arg);
void over_voltage_event_pins_init(void)
{
    gpio_clear(OVER_VOLTAGE_EVENT_PIN);
    gpio_init_int(OVER_VOLTAGE_EVENT_PIN, GPIO_IN, GPIO_RISING, OVER_VOLTAGE_EXIT2_IRQHandler, NULL);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    NVIC_DisableIRQ(EXTI2_IRQn);
}


void enable_over_voltage_irq(void)
{
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void disable_over_voltage_irq(void)
{
	NVIC_DisableIRQ(EXTI2_IRQn);
}


extern void PARTIAL_DISCHARGE_EXIT15_10_IRQHandler(void *arg);
void partial_discharge_event_pins_init(void)
{
    gpio_clear(PARTIAL_DISCHARGE_EVENT_PIN);
    gpio_init_int(PARTIAL_DISCHARGE_EVENT_PIN, GPIO_IN, GPIO_RISING, PARTIAL_DISCHARGE_EXIT15_10_IRQHandler, NULL);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    NVIC_DisableIRQ(EXTI15_10_IRQn);
}


void enable_partial_discharge_irq(void)
{
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void disable_partial_discharge_irq(void)
{
	NVIC_DisableIRQ(EXTI15_10_IRQn);
}

