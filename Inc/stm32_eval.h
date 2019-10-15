/**
  ******************************************************************************
  * @file    stm32_eval.h
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   Header file for stm32_eval.c module.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EVAL_H
#define __STM32_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


#ifdef USE_STM3210B_EVAL
 #include "stm3210b_eval.h"
#elif defined USE_STM3210E_EVAL
 #include "stm3210e_eval.h"
#elif defined USE_STM3210C_EVAL
 #include "stm3210c_eval.h"
#else 
 #error "Please select first the STM3210X_EVAL board to be used (in stm32_eval.h)"
#endif                      

#define CLI()      __set_PRIMASK(1)  
#define SEI()      __set_PRIMASK(0)  

#ifdef HIKE
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2
} Led_TypeDef;

typedef enum 
{  
	S1 = 0,
	S2 = 1,
	S3 = 2
} Button_TypeDef;

typedef enum 
{  
  Mode_GPIO = 0,
  Mode_EXTI = 1
} Button_Mode_TypeDef;
typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;

#else 
 #error "Please select first the STM3210X_EVAL board to be used (in stm32_eval.h)"
#endif //选择完毕开发板


extern GPIO_TypeDef* GPIO_PORT[];
extern uint16_t GPIO_PIN[];

extern void STM_EVAL_LEDInit(Led_TypeDef Led);
extern void STM_EVAL_LEDOn(Led_TypeDef Led);
extern void STM_EVAL_LEDOff(Led_TypeDef Led);
extern void STM_EVAL_LEDToggle(Led_TypeDef Led);
extern void STM_EVAL_PBInit(Button_TypeDef Button, Button_Mode_TypeDef Button_Mode);
extern uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
extern void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);

#ifdef __cplusplus
}
#endif




#endif 	//enddef stm32_eval.h

