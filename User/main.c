/**   
	 硬件版本：M新动力 STM32多路同步数据采集卡V1 
	 通信： USB转uart
	 供电： USB线 （注意：板卡右上角的黑色电源座为预留供电输入口，只能接入5V，内正外负；一般只需USB供电即可，改电源座不接）
	 跳线帽设置： P110、 P101接到下方（板子边缘）选择AD7606的并行16bit模式；   J10跳线帽短接；
	 拨码开关设置： 1、正常运行程序，拨码开关BOOT0和BOOT1均位于左侧； 2、串口下载模式，BOOT0位于右侧
	 复位按键：板子左下角的RST1
	 
   M新动力 STM32F103ZE 开发板控制AD7606例程，通过串口助手显示，波特率256000bps，格式8N1（8个数据位，没有奇偶校验，1个停止位）
	 AD7606的连接方式：FSMC并行16Bit； 
	 NE1片选，地址A4A3=00，选中片上AD7606的CS
	 通过 TIMx_CHx_PWM_CNAB（）函数触发CNAB的频率，即为采样频率; TIM1_CH1，PA8

   计算输入电压公式：Vin = CODE/32768*RANGE ,RANGE为 10 或者 5，CODE为有符号型 
	 
	 BUSY配置为下降沿中断，中断函数获取AD数值,采用串口调试助手进行AD7606数据的显示, 
	 注意：改程序试验程序，实际应用中不建议在中断函数里使用printf函数操作
	 
   通过 USART1_Configuration();配置STM32串口波特率，本例程默认为256000pbs，
	 （基础套餐一、二）板载的CH340 最高可达2Mbps；
	 （USB高速套餐）板载FT2232HL 最高10Mbps，完全可以满足STM32F103的4.5Mpbs最高串口速率
	  若不采用串口实时传输，可修改AD7606触发频率，进行更高速率的AD7606是采集试验；
	 	 

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
//LED初始化
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);

	
//按键初始化
	STM_EVAL_PBInit(S1,Mode_GPIO);	
  STM_EVAL_PBInit(S2,Mode_GPIO);	
	STM_EVAL_PBInit(S3,Mode_GPIO);	

	AD7606_IO_Init();//初始化AD7606与STM32的接口
	FSMC_Init();	   //初始化FSMC
	
	//设置触发AD采样频率
	TIMx_CHx_PWM_CNAB(); //2Hz采样频率

//------------------------------------------------------------------------->
	//串口1初始化
	USART1_Configuration(); //波特率 256000 8N1； 通过板载USB转uart芯片CH340  或者  FT2232HL的B通道作为USB转uart 与 PC进行通信

	AD7606.DMASend = 0;
	AD7606.R_index=0;
	AD7606.W_index=0;		
	
	//上电闪烁指示
	for(i=0;i<7;i++)
	{
		STM_EVAL_LEDToggle(LED1);
		Delay_ms(100);
	}	
	
	printf("\r\n STM32控制AD7606 多路数据采集测试");
	
	NVIC_Configuration();

	while (1)
	{
		  //PWM硬件触发CNAB，BUSY中断读取AD数据，通过串口上传到串口调试助手软件
		  //详见AD7606_FSMC.c 的 BUSY引脚中断函数  void EXTI4_IRQHandler(void)
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
