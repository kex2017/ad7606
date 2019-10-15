#include "include_all.h"

//------------------------------------------------------------------------->
//P90 : Cortex System Timer (SysTick) external clock with the AHB clock (HCLK) divided by 8.
//SysTick��HCLK/8�õ�������SysTick��ʱ��Ϊ9MHz
//SysTick��24bit�Ķ�ʱ��
//SysTick_ISR ���� Cortex-M3�쳣��������оƬ������Ƶ�Ӳ��

#ifdef SYSTICK_ISR_EN

//#################################################
//-----------------------------------------------
// Systick��ʱ����ʼ��
// ���� SystemCoreClock / 1000 ��ʾ 1ms �ж�
//-----------------------------------------------
void Systick_Init(void) 
{ 
    
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
}


//#################################################
//-----------------------------------------------
// SysTick�жϷ�����
//-----------------------------------------------
void SysTick_ISR(void)
{
	
}

#elif defined  SYSTICK_DELAY_EN
//#################################################
//-----------------------------------------------
// ms���ӳٺ�����ռ��SysTick��Դ
// SysTick��24bit�Ķ�ʱ��
// ��72Mhz�£�Nms��� 1864, ���� 0xFFFFFF > ((9MHz /1000) * ms) 
//-----------------------------------------------
void Delay_ms(uint16_t Nms)
{   

	SysTick->LOAD=(uint32_t)((SystemCoreClock/8000)*Nms);

	SysTick->CTRL|=(Stk_ENABLE);//��ʼ����   

	while(!(SysTick->CTRL&(1<<16)));      //�ȴ�ʱ�䵽��

	SysTick->CTRL&=~Stk_ENABLE;           //�رռ�����
	SysTick->VAL=0;         			        //��ռ�����    

}  



//#################################################
//-----------------------------------------------
// us���ӳٺ�����ռ��SysTick��Դ
// SysTick��24bit�Ķ�ʱ��
// ��72Mhz��,us���Ϊ 0xFFFFFF > ((9MHz /1000000) * us) 
//-----------------------------------------------
void Delay_us(u32 Nus)
{
	SysTick->LOAD=(u32)Nus*(SystemCoreClock/8000000);     

	SysTick->CTRL|=Stk_ENABLE;            //��ʼ����   

	while(!(SysTick->CTRL&(1<<16)));      //�ȴ�ʱ�䵽��

	SysTick->CTRL&=~Stk_ENABLE;          //�رռ�����	   	
	SysTick->VAL=0;                      //��ռ�����    

}

#endif 


