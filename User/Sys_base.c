#include "include_all.h"

//------------------------------------------------------------------------->
//P90 : Cortex System Timer (SysTick) external clock with the AHB clock (HCLK) divided by 8.
//SysTick是HCLK/8得到，故最SysTick大时钟为9MHz
//SysTick是24bit的定时器
//SysTick_ISR 属于 Cortex-M3异常，不属于芯片厂商设计的硬件

#ifdef SYSTICK_ISR_EN

//#################################################
//-----------------------------------------------
// Systick定时器初始化
// 参数 SystemCoreClock / 1000 标示 1ms 中断
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
// SysTick中断服务函数
//-----------------------------------------------
void SysTick_ISR(void)
{
	
}

#elif defined  SYSTICK_DELAY_EN
//#################################################
//-----------------------------------------------
// ms级延迟函数，占用SysTick资源
// SysTick是24bit的定时器
// 在72Mhz下，Nms最大 1864, 满足 0xFFFFFF > ((9MHz /1000) * ms) 
//-----------------------------------------------
void Delay_ms(uint16_t Nms)
{   

	SysTick->LOAD=(uint32_t)((SystemCoreClock/8000)*Nms);

	SysTick->CTRL|=(Stk_ENABLE);//开始倒数   

	while(!(SysTick->CTRL&(1<<16)));      //等待时间到达

	SysTick->CTRL&=~Stk_ENABLE;           //关闭计数器
	SysTick->VAL=0;         			        //清空计数器    

}  



//#################################################
//-----------------------------------------------
// us级延迟函数，占用SysTick资源
// SysTick是24bit的定时器
// 在72Mhz下,us最大为 0xFFFFFF > ((9MHz /1000000) * us) 
//-----------------------------------------------
void Delay_us(u32 Nus)
{
	SysTick->LOAD=(u32)Nus*(SystemCoreClock/8000000);     

	SysTick->CTRL|=Stk_ENABLE;            //开始倒数   

	while(!(SysTick->CTRL&(1<<16)));      //等待时间到达

	SysTick->CTRL&=~Stk_ENABLE;          //关闭计数器	   	
	SysTick->VAL=0;                      //清空计数器    

}

#endif 


