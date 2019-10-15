/**
  ******************************************************************************
  * @file    stm32_eval.c
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   This file provides firmware functions to manage Leds, push-buttons
  *          and COM ports available on STM32 Evaluation Boards from STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"

//------------------------------------------------------------------------->
//LED
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT};
 uint16_t GPIO_PIN[LEDn] = {LED1_GPIO_PIN, LED2_GPIO_PIN, LED3_GPIO_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK};

//------------------------------------------------------------------------->
//按键
 GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {S1_PORT, S2_PORT, S3_PORT};

 const uint16_t BUTTON_PIN[BUTTONn] = {S1_PIN,S2_PIN, S3_PIN};

 const uint32_t BUTTON_CLK[BUTTONn] = {S1_CLK,S2_CLK, S3_CLK};

 const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {S1_EXTI_LINE,
                                             S2_EXTI_LINE,
                                             S3_EXTI_LINE};

 const uint16_t BUTTON_PORT_SOURCE[BUTTONn] = {S1_PORT_SOURCE,
                                               S2_PORT_SOURCE,
                                               S3_PORT_SOURCE};

 const uint16_t BUTTON_PIN_SOURCE[BUTTONn] = {S1_PIN_SOURCE,
                                              S2_PIN_SOURCE,
                                              S3_PIN_SOURCE};

 const uint16_t BUTTON_IRQn[BUTTONn] = {S1_IRQn,
																				S2_IRQn, 
																				S3_IRQn};

										
//#################################################
//-----------------------------------------------
// LED初始化
// 参数为 Led_TypeDef枚举里的LED0、LED1、LED2、LED3
//-----------------------------------------------
void STM_EVAL_LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  STM_EVAL_LEDOff(Led);

  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}


//#################################################
//-----------------------------------------------
// 点亮哪颗LED
// 参数为 Led_TypeDef枚举里的LED0、LED1、LED2、LED3
//-----------------------------------------------
void STM_EVAL_LEDOn(Led_TypeDef Led)
{
  #if 1
	GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
  #else
	GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
  #endif
}

//#################################################
//-----------------------------------------------
// 熄灭哪颗LED
// 参数为 Led_TypeDef枚举里的LED1、LED2、LED3
//-----------------------------------------------
void STM_EVAL_LEDOff(Led_TypeDef Led)
{
  #if 1
		GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
  #else
		GPIO_PORT[Led]->BRR = GPIO_PIN[Led];  //复位
  #endif
}

//#################################################
//-----------------------------------------------
// 翻转哪颗LED
// 参数为 Led_TypeDef枚举里的LED0、LED1、LED2、LED3
//-----------------------------------------------
void STM_EVAL_LEDToggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}


//#################################################
//-----------------------------------------------
// 按键初始化
// Button制定哪个按键；Button_Mode为GPIO方式还是中断方式
//-----------------------------------------------
void STM_EVAL_PBInit(Button_TypeDef Button, Button_Mode_TypeDef Button_Mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable Button GPIO clock */
  RCC_APB2PeriphClockCmd(BUTTON_CLK[Button] | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);

  if (Button_Mode == Mode_EXTI)
  {
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(BUTTON_PORT_SOURCE[Button], BUTTON_PIN_SOURCE[Button]);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

//#ifdef HIKE
//	if(Button != S2)
//    {
//      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//    }
//#else
//	if(Button != Button_WAKEUP)
//    {
//      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//    }
//#endif
//    else
//    {
//      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//    }
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //Enable and set Button EXTI Interrupt to the lowest priority
    NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);	

  }
}

//#################################################
//-----------------------------------------------
// 获取按键值
// Button制定哪个按键
//-----------------------------------------------
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button)
{
  return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
