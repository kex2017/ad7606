#include "include_all.h"


Mark_Para uart1Mark;
UART_parameter Uart1;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */




//void USART1_Configuration(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	USART_TypeDef *uart;


//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);


//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);


//	uart = USART1;	
//	USART_InitStructure.USART_BaudRate = 115200 ;//1500000; 900000
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(uart, &USART_InitStructure);
//	USART_ITConfig(uart, USART_IT_RXNE, ENABLE);	

//	USART_Cmd(uart, ENABLE);	
//	USART_ClearFlag(uart, USART_FLAG_TC);   
//}

//void UartSend(uint8_t ch)
//{
//	USART1->DR = ch;
//	while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
//}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

//#################################################
//-----------------------------------------------
// 串口1接收中断
// 接收到的数据放在Uart1.RxBuf[]里
//-----------------------------------------------
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  	{
      uart1Mark.Status_Bits.RxIntFlag = 1;
			Uart1.RxBuf[0] = USART_ReceiveData(USART1);

			USART_SendData(USART1,Uart1.RxBuf[0]);	//通过 串口1 将数据回传
/*
			if(Uart1.RxCounter == RxBUFLED)  					              
			{
				Uart1.RxCounter=0;			
				uart1Mark.Status_Bits.RxBufFull = 1;				         //缓冲区满标志	
			}
*/			
		}
	else
		{
 	 		USART_ClearITPendingBit(USART1,USART_IT_CTS|USART_IT_LBD|USART_IT_TC|USART_IT_RXNE);
			USART_ClearFlag(USART1,USART_IT_CTS|USART_IT_LBD|USART_IT_TC|USART_IT_RXNE);
		}	
}


