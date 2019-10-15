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

 
	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* ��4��������USART����
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

	USART_DMACmd(USARTy, USART_DMAReq_Tx, ENABLE);	//����һ�μ���,DMA ������

	// CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ;�����������1���ֽ��޷���ȷ���ͳ�ȥ������ 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);     //�巢����Ǳ�־��Transmission Complete flag 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
 
	DMA_DeInit(USARTy_Tx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USARTy_DR_Base;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Txsting;//�� StartOneDMATx_Configuration()����������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = DMA_send_size_BYTE; //�� StartOneDMATx_Configuration()����������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//��Normal ģʽ�£���������һ�δ��䣬��
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
	DMA_CHx	-> CCR &=~ (1<<0);	   //�ر�DMA
	DMA_CHx -> CNDTR = BYTE_LEN ; //DMA1 ����������  DMA_send_size_BYTE
	DMA_CHx -> CMAR = (uint32_t)MEMADDR;
	DMA_CHx -> CCR |= (1<<0);			//����DMA			 

}
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void DMA1_Channel4_IRQHandler(void)
{

}
////////////////////////////////////////////////////////////////////////////////////////////
