/**   
	 Ӳ���汾��M�¶��� STM32��·ͬ�����ݲɼ���V1 
	 ͨ�ţ� USBתuart
	 ���磺 USB�� ��ע�⣺�忨���Ͻǵĺ�ɫ��Դ��ΪԤ����������ڣ�ֻ�ܽ���5V�������⸺��һ��ֻ��USB���缴�ɣ��ĵ�Դ�����ӣ�
	 ����ñ���ã� P110�� P101�ӵ��·������ӱ�Ե��ѡ��AD7606�Ĳ���16bitģʽ��   J10����ñ�̽ӣ�
	 ���뿪�����ã� 1���������г��򣬲��뿪��BOOT0��BOOT1��λ����ࣻ 2����������ģʽ��BOOT0λ���Ҳ�
	 ��λ�������������½ǵ�RST1
	 
   M�¶��� STM32F103ZE ���������AD7606���̣�ͨ������������ʾ��������256000bps����ʽ8N1��8������λ��û����żУ�飬1��ֹͣλ��
	 AD7606�����ӷ�ʽ��FSMC����16Bit�� 
	 NE1Ƭѡ����ַA4A3=00��ѡ��Ƭ��AD7606��CS
	 ͨ�� TIMx_CHx_PWM_CNAB������������CNAB��Ƶ�ʣ���Ϊ����Ƶ��; TIM1_CH1��PA8

   ���������ѹ��ʽ��Vin = CODE/32768*RANGE ,RANGEΪ 10 ���� 5��CODEΪ�з����� 
	 
	 BUSY����Ϊ�½����жϣ��жϺ�����ȡAD��ֵ,���ô��ڵ������ֽ���AD7606���ݵ���ʾ, 
	 ע�⣺�ĳ����������ʵ��Ӧ���в��������жϺ�����ʹ��printf��������
	 
   ͨ�� USART1_Configuration();����STM32���ڲ����ʣ�������Ĭ��Ϊ256000pbs��
	 �������ײ�һ���������ص�CH340 ��߿ɴ�2Mbps��
	 ��USB�����ײͣ�����FT2232HL ���10Mbps����ȫ��������STM32F103��4.5Mpbs��ߴ�������
	  �������ô���ʵʱ���䣬���޸�AD7606����Ƶ�ʣ����и������ʵ�AD7606�ǲɼ����飻
	 	 

/* Includes ------------------------------------------------------------------*/

#include "include_all.h"

void NVIC_Configuration(void);
void RCC_Configuration(void);

int i;

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */
//////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------->
//LED��ʼ��
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);

	
//������ʼ��
	STM_EVAL_PBInit(S1,Mode_GPIO);	
  STM_EVAL_PBInit(S2,Mode_GPIO);	
	STM_EVAL_PBInit(S3,Mode_GPIO);	

	AD7606_IO_Init();//��ʼ��AD7606��STM32�Ľӿ�
	FSMC_Init();	   //��ʼ��FSMC
	
	//���ô���AD����Ƶ��
	TIMx_CHx_PWM_CNAB(); //2Hz����Ƶ��

//------------------------------------------------------------------------->
	//����1��ʼ��
	USART1_Configuration(); //������ 256000 8N1�� ͨ������USBתuartоƬCH340  ����  FT2232HL��Bͨ����ΪUSBתuart �� PC����ͨ��

	AD7606.DMASend = 0;
	AD7606.R_index=0;
	AD7606.W_index=0;		
	
	//�ϵ���˸ָʾ
	for(i=0;i<7;i++)
	{
		STM_EVAL_LEDToggle(LED1);
		Delay_ms(100);
	}	
	
	printf("\r\n STM32����AD7606 ��·���ݲɼ�����");
	
	NVIC_Configuration();

	while (1)
	{
		  //PWMӲ������CNAB��BUSY�ж϶�ȡAD���ݣ�ͨ�������ϴ������ڵ����������
		  //���AD7606_FSMC.c �� BUSY�����жϺ���  void EXTI4_IRQHandler(void)
	}
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=BUSY_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			
	
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif




/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
