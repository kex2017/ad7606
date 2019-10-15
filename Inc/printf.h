#ifndef __MYPRINTF_H
#define __MYPRINTF_H

#define TXBUFLED 10
#define RxBUFLED 10

//------------------------------------------------------------------------->
//���������Ҫ�ı�־λ������������������16����־λ
typedef union
{
  uint16_t All;
  struct{
					uint16_t  RxIntFlag         :1; //�����жϱ�־λ
					uint16_t  RxBufFull         :1; //���ջ���������־λ

        }Status_Bits;
}Mark_Para;  

extern Mark_Para uart1Mark;


typedef struct
{
	uint8_t  TxBuf[TXBUFLED];			//���ͻ����� 
	uint16_t TxCounter;
	
	uint8_t  RxBuf[RxBUFLED];			//���ջ����� 
	uint16_t RxCounter;
	
}UART_parameter;
extern UART_parameter Uart1;

void USART1_Configuration(void);

//void UartSend(uint8_t ch);

#define UartSend(ch) while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);USART1->DR = ch;



#endif
