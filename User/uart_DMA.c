#include "stm32f10x.h"
#include "uart_DMA.h"
#include <string.h>
#include <stdio.h>



USART_InitTypeDef USART_InitStructure;

//////////////////////////////////////////////////////////////////////////////////////////
void uart_DMA_RRC_IO_NVIC_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(USARTy_CLK, ENABLE); 
												   
}
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

 
	/* 第1步：打开GPIO和USART部件的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  第3步已经做了，因此这步可以不做
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* 第4步：配置USART参数
	    - BaudRate = 115200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - No parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 256000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;


	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);


	USART_ClearITPendingBit(USART1,USART_IT_CTS|USART_IT_LBD|USART_IT_TC|USART_IT_RXNE);


	USART_Cmd(USART1, ENABLE);

	USART_DMACmd(USARTy, USART_DMAReq_Tx, ENABLE);	//开启一次即可,DMA 操作用

	// CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去;如下语句解决第1个字节无法正确发送出去的问题 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);     //清发送外城标志，Transmission Complete flag 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
 
	DMA_DeInit(USARTy_Tx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USARTy_DR_Base;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Txsting;//在 StartOneDMATx_Configuration()函数里配置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = DMA_send_size_BYTE; //在 StartOneDMATx_Configuration()函数里配置
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//在Normal 模式下，与启动下一次传输，则
												//disable the DMA channel ,reload a new number into the DMA_SNDTRx
  	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  	DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);

//	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);                               
							 
	DMA_Cmd(USARTy_Tx_DMA_Channel, ENABLE);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void StartOneDMATx_Configuration(DMA_Channel_TypeDef * DMA_CHx, uint16_t BYTE_LEN,  uint32_t * MEMADDR) 
{
	DMA_CHx	-> CCR &=~ (1<<0);	   //关闭DMA
	DMA_CHx -> CNDTR = BYTE_LEN ; //DMA1 传输数据量  DMA_send_size_BYTE
	DMA_CHx -> CMAR = (uint32_t)MEMADDR;
	DMA_CHx -> CCR |= (1<<0);			//开启DMA			 

}
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DMA1_Channel4_IRQHandler(void)
{

}
////////////////////////////////////////////////////////////////////////////////////////////
