#include "include_all.h"


//typedef struct
//{
//	uint16_t  CHn[8][32];//8��ͨ����ÿ�����32
//	uint16_t  R_index;   //д��CHn ����� ����
//	uint16_t  W_index;   //����CHn ����� ����
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

//����FSMC���ʵ�ַ AD7606 
//A4 A3 = 00
#define AD7606_BASE        ((uint32_t)(0x60000000 ))
#define AD7606_FSMC        ((AD7606_TypeDef *) AD7606_BASE)


//#################################################
//-----------------------------------------------
//FSMC ��ʼ��
//-----------------------------------------------
void FSMC_Init(void)
{
	AD7606_CtrlLinesConfig();	//FSMC IO��ʼ��
	AD7606_FSMCConfig();      //FSMC ��������ʼ��
	Delay_ms(20);
}


//#################################################
//-----------------------------------------------
//FSMC IO��ʼ��
//AD7606��ռFSMC���ų�ʼ��
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
		PF3/FSMC_A3		--- ����Ƭѡһ������
		PF4/FSMC_A4		--- ����Ƭѡһ������
	*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;													
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

}
//#################################################
//-----------------------------------------------
//FSMC��������ʼ��
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
//AD7606�������š��������ų�ʼ��
//��λAD7606
//��������10V�������̣��޹�����
//REF_SEL���ⲿ��������ѡ���ڲ��ο�Դ
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
	
//  CNAB��Ӳ��TIM4_CH4 PWM Ӳ������	�����ﲻ��GPIO����
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
	
	RST_H();															//�ߵ�ƽ��λAD7606
			
  //BUSY������Ϊ�ж�
	GPIO_InitStructure.GPIO_Pin = BUSY_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(BUSY_PORT, &GPIO_InitStructure);	
	GPIO_EXTILineConfig(BUSY_PORT_SOURCE, BUSY_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = BUSY_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//��ʼ��Ĭ�ϵĿ��Ƶ�ƽ             
	RANGE_H();							// ģ��������10V����
	OS0_L();OS1_L();OS2_L();//�������ر�
  RST_L();                //��ɶ�AD7606�ĸ�λ���ظ��͵�ƽ
}



//#################################################
//CRC У��
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
//  ���������ʽ--��ʱ����ʼ��
//  �������� = 72M /(TIM_Prescaler+1)/(TIM_Period+1)
//  ������Ϊ2.5K
//  2K = 72M / (499+1)/(71+1)
//-----------------------------------------------
void Timer4Init(void)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//��ʼ������ʱ��
	
	//ϵͳʱ�� SystemCoreClock ==72Mhz ,�ⲿ���� 8�� PLL
  TIM_TimeBaseStructure.TIM_Prescaler = 9999;  // TIMx prescaler (TIMx_PSC)�Ĵ��� 499
	TIM_TimeBaseStructure.TIM_Period = 7199;	//TIMx prescaler (TIMx_PSC)�Ĵ���

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	     //used by the digital filters
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ�ʱ
	

 	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);	//������ʱ������

	TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);	      //�����ж�

	TIM_Cmd(TIM4, ENABLE);												  //ʹ�ܶ�ʱ��

}


//#################################################
//-----------------------------------------------
//�����ʽ����CNAB�����Ƽ�
//��ʱ������CNA CNB����AD7606����
//-----------------------------------------------
void TIM4_IRQHandler(void)
{ 	
//	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	printf("\r\nTIM4");
		TIM4->SR = (uint16_t)~TIM_IT_Update;	
//	if(AD7606.DMASend==0)
	{
		//����AD�ɼ� CNA CNB ����һ��������
		CNAB_L();
		CNAB_H();;
	}
}

//#################################################
//-----------------------------------------------
//Ӳ����ʽ����CNA CNB  ��·��ӵ�J1 �� �Ҳ�  
//��ʱ��TIM1 CH1(PA8) ��ΪPWM����������崥��AD7606 CNAB�ź� 
//����Ƶ���� PrescalerValue ����
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
	
	//��ʱ��TIM1 CH1(PA8) ��ΪPWM����������崥��AD7606 CNAB�ź�  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//����Ƶ�ʼ��㣺 72000000/��TIM_Period+1��/��PrescalerValue+1�� 
	
	//��TIM_Period=1ʱ��179 = 200K; 199 = 180K ; 239 = 150K ; 359=100K ; 449=80k; 899=40K; 3599=10k; 11999=3K; 35999 = 1k
	//��TIM_Period = 999ʱ��PrescalerValue = 35999=2Hz
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
//BUSY �����½����жϣ�����FSMC AD��ֵ�Ķ�ȡ
//������ȡ8��ͨ��
//����ASCII��ʽ���������ϴ������ô��ڵ������ֲ鿴����  ������ 256000pbs
//���������ѹ��ʽ��Vin = CODE/32768*RANGE ,RANGEΪ 10 ���� 5��CODEΪ�з����� 
//-----------------------------------------------
void EXTI4_IRQHandler(void)
{
	char i;
  uint32_t enablestatus = 0; 

  enablestatus =  EXTI->IMR & BUSY_EXTI_LINE;
//	if(EXTI_GetITStatus(BUSY_EXTI_LINE) != RESET)	
  if (((EXTI->PR & BUSY_EXTI_LINE) != (uint32_t)RESET) && (enablestatus != (uint32_t)RESET))
	{
			//��ȡ8��ͨ������
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
				printf("  CH[%d]ԭʼ%0.4x||��ѹ%0.3f",i+1,(int)AD7606.CHn[i][AD7606.W_index]&0xFFFF,(float)AD7606.CHn[i][AD7606.W_index]/32768*10);//����ű�0Ϊ CH1����������
			}
			printf("\r\n");
			
			AD7606.W_index = (AD7606.W_index==(DATA_LEN-1))?0:AD7606.W_index+1;     //д��������

		//�����ֶ� Clears the EXTI's line pending bits���������˳��жϷ��������ֽ���˺���������ѭ��  
		 EXTI->PR = BUSY_EXTI_LINE;
	}
}

