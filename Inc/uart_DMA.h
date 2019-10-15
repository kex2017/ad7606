/**
  ******************************************************************************
  * @file    USART/DMA_Interrupt/platform_config.h 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   Evaluation board specific configuration file.
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


#ifndef __UART_DMA_H
#define __UART_DMA_H



#define USARTy                   USART1
#define USARTy_GPIO              GPIOA
#define USARTy_CLK               RCC_APB2Periph_USART1
#define USARTy_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTy_RxPin             GPIO_Pin_10
#define USARTy_TxPin             GPIO_Pin_9
#define USARTy_Tx_DMA_Channel    DMA1_Channel4
#define USARTy_Tx_DMA_FLAG       DMA1_FLAG_TC4
#define USARTy_DR_Base           0x40013804



#define DMA_send_size_BYTE 24 //MAX IS 65535

extern uint16_t Txsting[];
extern uint16_t *P_buf;;

void DMA1_Channel4_IRQHandler(void);
extern void uart_DMA_RRC_IO_NVIC_init(void);
extern void DMA_Configuration(void);
extern void USART1_Configuration(void);
extern void StartOneDMATx_Configuration(DMA_Channel_TypeDef * DMA_CHx,uint16_t BYTE_LEN,uint32_t *MEMADDR);


#endif /* __UART_DMA_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
