#include "timer.h"
#include "rs485.h"
#include "stm32f4xx.h"
#include "LED.h"
void TIM3_Init(u16 RS485_Frame_Distance)
{  
	 uint16_t Prescaler = 0;
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM3时钟使能 
 
	 Prescaler=(uint16_t)((SystemCoreClock / 10000) - 1);
        //TIM7初始化设置
   TIM_TimeBaseStructure.TIM_Period = 10*RS485_Frame_Distance; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
   TIM_TimeBaseStructure.TIM_Prescaler =Prescaler; //设置用来作为TIMx时钟频率除数的预分频值 设置计数频率为10kHz
   TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	 TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	// TIM_Cmd(TIM3,ENABLE); //使能定时器3
 
         //TIM7中断分组配置
   NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;  //TIM3中断
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
   NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器  
}

void TIM3_IRQHandler()
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		 TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//清除中断标志
     TIM_Cmd(TIM3,DISABLE);//停止定时器
     RS485_TX_EN=1;//停止接收，切换为发送状态
     RS485_FrameFlag=1;//置位帧结束标记
		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位

}