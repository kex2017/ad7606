#ifndef __MYPRINTF_H
#define __MYPRINTF_H

#define TXBUFLED 10
#define RxBUFLED 10

//------------------------------------------------------------------------->
//继续添加想要的标志位，该联合体最多可以有16个标志位
typedef union
{
  uint16_t All;
  struct{
					uint16_t  RxIntFlag         :1; //接收中断标志位
					uint16_t  RxBufFull         :1; //接收缓冲区满标志位

        }Status_Bits;
}Mark_Para;  

extern Mark_Para uart1Mark;


typedef struct
{
	uint8_t  TxBuf[TXBUFLED];			//发送缓冲区 
	uint16_t TxCounter;
	
	uint8_t  RxBuf[RxBUFLED];			//接收缓冲区 
	uint16_t RxCounter;
	
}UART_parameter;
extern UART_parameter Uart1;

void USART1_Configuration(void);

//void UartSend(uint8_t ch);

#define UartSend(ch) while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);USART1->DR = ch;



#endif
