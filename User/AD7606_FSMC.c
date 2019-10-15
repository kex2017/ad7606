#include "include_all.h"


//typedef struct
//{
//	uint16_t  CHn[8][32];//8个通道，每个深度32
//	uint16_t  R_index;   //写入CHn 数组的 索引
//	uint16_t  W_index;   //读出CHn 数组的 索引
//	
//}AD7606Para;

AD7606Para AD7606 ={{0},0,0};


int CRCCode;
unsigned char AD_Uart[30];


void AD7606_GPIO_Init(void);
static void AD7606_CtrlLinesConfig(void);
static void AD7606_FSMCConfig(void);


typedef struct
{
__IO uint16_t Write;
__IO uint16_t Read;
}
AD7606_TypeDef;

//定义FSMC访问地址 AD7606 
//A4 A3 = 00
#define AD7606_BASE        ((uint32_t)(0x60000000 ))
#define AD7606_FSMC        ((AD7606_TypeDef *) AD7606_BASE)


//#################################################
//-----------------------------------------------
//FSMC 初始化
//-----------------------------------------------
void FSMC_Init(void)
{
	AD7606_CtrlLinesConfig();	//FSMC IO初始化
	AD7606_FSMCConfig();      //FSMC 控制器初始化
	Delay_ms(20);
}


//#################################################
//-----------------------------------------------
//FSMC IO初始化
//AD7606所占FSMC引脚初始化
//-----------------------------------------------
void AD7606_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF|
	                  RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
	                        GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
	                        GPIO_Pin_15|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	

//  GPIO_PinRemapConfig();
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                        GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                        GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;													
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
		/*
		PF3/FSMC_A3		--- 和主片选一起译码
		PF4/FSMC_A4		--- 和主片选一起译码
	*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;													
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

}
//#################################################
//-----------------------------------------------
//FSMC控制器初始化
//NE1 for AD7606
//-----------------------------------------------
void AD7606_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef  
	FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;
	
	/*-- FSMC Configuration ------------------------------------------------------*/
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 1;
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 2;
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
	
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1; //BANK1 RAM1
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	
	/* - BANK 1 (of NOR/SRAM Bank 0~3) is enabled */
		FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}


uint16_t FSMC_AD7606_RD(void)
{
	return AD7606_FSMC->Read;	
}


//#################################################
//-----------------------------------------------
//AD7606控制引脚、数据引脚初始化
//复位AD7606
//配置正负10V输入量程，无过采样
//REF_SEL由外部电阻拉高选择内部参考源
//-----------------------------------------------
void AD7606_IO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
  RCC_APB2PeriphClockCmd(OS0_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = OS0_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(OS0_PORT, &GPIO_InitStructure);
	
  RCC_APB2PeriphClockCmd(OS1_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = OS1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(OS1_PORT, &GPIO_InitStructure);
	
  RCC_APB2PeriphClockCmd(OS2_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = OS2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(OS2_PORT, &GPIO_InitStructure);
	
//  CNAB由硬件TIM4_CH4 PWM 硬件触发	，这里不做GPIO控制
//  RCC_APB2PeriphClockCmd(CNAB_CLK, ENABLE);
//  GPIO_InitStructure.GPIO_Pin = CNAB_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(CNAB_PORT, &GPIO_InitStructure);
	
  RCC_APB2PeriphClockCmd(RST_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RST_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RST_PORT, &GPIO_InitStructure);
	
  RCC_APB2PeriphClockCmd(RANGE_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RANGE_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RANGE_PORT, &GPIO_InitStructure);
	
	RST_H();															//高电平复位AD7606
			
  //BUSY引脚作为中断
	GPIO_InitStructure.GPIO_Pin = BUSY_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(BUSY_PORT, &GPIO_InitStructure);	
	GPIO_EXTILineConfig(BUSY_PORT_SOURCE, BUSY_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = BUSY_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//初始化默认的控制电平             
	RANGE_H();							// 模拟量正负10V输入
	OS0_L();OS1_L();OS2_L();//过采样关闭
  RST_L();                //完成对AD7606的复位，回复低电平
}



//#################################################
//CRC 校验
//-----------------------------------------------
unsigned int GetCRC16(unsigned char *inPtr, unsigned int len) {
	unsigned int crc = 0xffff;
	unsigned char index;

	while (len > 0) {
		crc ^= *inPtr;
		for (index = 0; index < 8; index++) {
			if ((crc & 0x0001) == 0)
				crc = crc >> 1;
			else {
				crc = crc >> 1;
				crc ^= 0xa001;
			}
		}

		len -= 1;
		inPtr++;

	}
	return (((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8));
}



//#################################################
//-----------------------------------------------
//  软件触发方式--定时器初始化
//  采样周期 = 72M /(TIM_Prescaler+1)/(TIM_Period+1)
//  最大采样为2.5K
//  2K = 72M / (499+1)/(71+1)
//-----------------------------------------------
void Timer4Init(void)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//初始化外设时钟
	
	//系统时钟 SystemCoreClock ==72Mhz ,外部晶振 8兆 PLL
  TIM_TimeBaseStructure.TIM_Prescaler = 9999;  // TIMx prescaler (TIMx_PSC)寄存器 499
	TIM_TimeBaseStructure.TIM_Period = 7199;	//TIMx prescaler (TIMx_PSC)寄存器

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	     //used by the digital filters
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计时
	

 	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);	//基本定时器功能

	TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);	      //配置中断

	TIM_Cmd(TIM4, ENABLE);												  //使能定时器

}


//#################################################
//-----------------------------------------------
//软件方式触发CNAB，不推荐
//定时器触发CNA CNB进行AD7606采样
//-----------------------------------------------
void TIM4_IRQHandler(void)
{ 	
//	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	printf("\r\nTIM4");
		TIM4->SR = (uint16_t)~TIM_IT_Update;	
//	if(AD7606.DMASend==0)
	{
		//触发AD采集 CNA CNB 产生一个低脉冲
		CNAB_L();
		CNAB_H();;
	}
}

//#################################################
//-----------------------------------------------
//硬件方式触发CNA CNB  短路块接到J1 的 右侧  
//定时器TIM1 CH1(PA8) 作为PWM输出，低脉冲触发AD7606 CNAB信号 
//触发频率由 PrescalerValue 决定
//TIM1_CH1
//-----------------------------------------------
void TIMx_CHx_PWM_CNAB()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t PrescalerValue = 0;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//定时器TIM1 CH1(PA8) 作为PWM输出，低脉冲触发AD7606 CNAB信号  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//采样频率计算： 72000000/（TIM_Period+1）/（PrescalerValue+1） 
	
	//当TIM_Period=1时；179 = 200K; 199 = 180K ; 239 = 150K ; 359=100K ; 449=80k; 899=40K; 3599=10k; 11999=3K; 35999 = 1k
	//当TIM_Period = 999时；PrescalerValue = 35999=2Hz
  PrescalerValue = 35999;
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM1;//TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	
  TIM_OCInitStructure.TIM_Pulse = 1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
//  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
//  TIM_ARRPreloadConfig(TIM1, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

//#################################################
//-----------------------------------------------
//BUSY 引脚下降沿中断，进行FSMC AD数值的读取
//连续读取8个通道
//采用ASCII格式进行数据上传，采用串口调试助手查看数据  波特率 256000pbs
//计算输入电压公式：Vin = CODE/32768*RANGE ,RANGE为 10 或者 5，CODE为有符号型 
//-----------------------------------------------
void EXTI4_IRQHandler(void)
{
	char i;
  uint32_t enablestatus = 0; 

  enablestatus =  EXTI->IMR & BUSY_EXTI_LINE;
//	if(EXTI_GetITStatus(BUSY_EXTI_LINE) != RESET)	
  if (((EXTI->PR & BUSY_EXTI_LINE) != (uint32_t)RESET) && (enablestatus != (uint32_t)RESET))
	{
			//读取8个通道数据
		  AD7606.W_index=0;
			AD7606.CHn[0][AD7606.W_index] = AD7606_FSMC->Read;	
			AD7606.CHn[1][AD7606.W_index] = AD7606_FSMC->Read;	
			AD7606.CHn[2][AD7606.W_index] = AD7606_FSMC->Read;	
			AD7606.CHn[3][AD7606.W_index] = AD7606_FSMC->Read;
			AD7606.CHn[4][AD7606.W_index] = AD7606_FSMC->Read;	
			AD7606.CHn[5][AD7606.W_index] = AD7606_FSMC->Read;	
			AD7606.CHn[6][AD7606.W_index] = AD7606_FSMC->Read;
			AD7606.CHn[7][AD7606.W_index] = AD7606_FSMC->Read;	
		
		  STM_EVAL_LEDToggle(LED2);
		  for(i=0;i<8;i++)
			{
				printf("  CH[%d]原始%0.4x||电压%0.3f",i+1,(int)AD7606.CHn[i][AD7606.W_index]&0xFFFF,(float)AD7606.CHn[i][AD7606.W_index]/32768*10);//数组脚标0为 CH1，依次类推
			}
			printf("\r\n");
			
			AD7606.W_index = (AD7606.W_index==(DATA_LEN-1))?0:AD7606.W_index+1;     //写索引增加

		//必须手动 Clears the EXTI's line pending bits，否者在退出中断服务函数后又进入此函数，无限循环  
		 EXTI->PR = BUSY_EXTI_LINE;
	}
}

