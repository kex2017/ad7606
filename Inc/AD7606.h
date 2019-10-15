#ifndef __ADS8365_H  
#define __ADS8365_H


#define DATA_LEN 128  //数组深度
//------------------------------------------------------------------------->
//四个按键的外部中断标志位
typedef struct
{
	short int  CHn[8][DATA_LEN];//8个通道，每个深度 DATA_LEN
	uint16_t  R_index;   //写入CHn 数组的 索引
	uint16_t  W_index;   //读出CHn 数组的 索引
	char DMASend ;
	
}AD7606Para;

extern AD7606Para AD7606;

extern int CRCCode;
extern unsigned char AD_Uart[];


#define FIFO_LEN 14
#define DMA_TX_NUM  10

//输入
#define BUSY_PORT          GPIOG
#define BUSY_CLK           RCC_APB2Periph_GPIOG
#define BUSY_PIN           GPIO_Pin_4
#define BUSY_EXTI_LINE     EXTI_Line4
#define BUSY_PORT_SOURCE   GPIO_PortSourceGPIOG
#define BUSY_PIN_SOURCE    GPIO_PinSource4
#define BUSY_IRQn          EXTI4_IRQn 
#define BUSY_READ()        GPIO_ReadInputDataBit(BUSY_PORT,  BUSY_PIN)



//输出
#define OS0_PORT  	GPIOG
#define OS0_CLK		  RCC_APB2Periph_GPIOG
#define OS0_PIN   	GPIO_Pin_8
#define OS0_H()   	GPIO_SetBits(OS0_PORT,OS0_PIN);	
#define OS0_L()   	GPIO_ResetBits(OS0_PORT,OS0_PIN); 

#define OS1_PORT  	GPIOG
#define OS1_CLK		  RCC_APB2Periph_GPIOG
#define OS1_PIN   	GPIO_Pin_7
#define OS1_H()   	GPIO_SetBits(OS1_PORT,OS1_PIN);	
#define OS1_L()   	GPIO_ResetBits(OS1_PORT,OS1_PIN); 

#define OS2_PORT  	GPIOG
#define OS2_CLK		  RCC_APB2Periph_GPIOG
#define OS2_PIN   	GPIO_Pin_6
#define OS2_H()   	GPIO_SetBits(OS2_PORT,OS2_PIN);	
#define OS2_L()   	GPIO_ResetBits(OS2_PORT,OS2_PIN); 

#define RANGE_PORT  	GPIOG
#define RANGE_CLK	    RCC_APB2Periph_GPIOG
#define RANGE_PIN   	GPIO_Pin_5
#define RANGE_H()   	GPIO_SetBits(RANGE_PORT,RANGE_PIN);	
#define RANGE_L()   	GPIO_ResetBits(RANGE_PORT,RANGE_PIN); 

#define CNAB_PORT  	GPIOA
#define CNAB_CLK		RCC_APB2Periph_GPIOA
#define CNAB_PIN   	GPIO_Pin_8
#define CNAB_H()   	GPIO_SetBits(CNAB_PORT,CNAB_PIN);	
#define CNAB_L()   	GPIO_ResetBits(CNAB_PORT,CNAB_PIN); 

#define RST_PORT  	GPIOD
#define RST_CLK		  RCC_APB2Periph_GPIOD
#define RST_PIN   	GPIO_Pin_11
#define RST_H()   	GPIO_SetBits(RST_PORT,RST_PIN);	
#define RST_L()   	GPIO_ResetBits(RST_PORT,RST_PIN); 



extern char String_Test[];
extern char String_SPECE[];

void AD7606_IO_Init(void);
void TIMx_CHx_PWM_CNAB(void);
void Timer4Init(void);
unsigned int GetCRC16(unsigned char *inPtr, unsigned int len);

void FSMC_NE2_CtrlLinesConfig(void);
void FSMC_NE2_FSMCConfig(void);
uint16_t FSMC_AD7606_RD(void);

void FSMC_Init(void);
void Key_Scan(void);
void EXTI3_IRQHandler(void);

#endif 	 //ADS8365_H
