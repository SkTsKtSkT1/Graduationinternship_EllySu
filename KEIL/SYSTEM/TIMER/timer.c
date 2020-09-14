#include "timer.h"
#include "rs485.h"
#include "stm32f4xx.h"
#include "LED.h"
void TIM3_Init(u16 RS485_Frame_Distance)
{  
	 uint16_t Prescaler = 0;
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM3ʱ��ʹ�� 
 
	 Prescaler=(uint16_t)((SystemCoreClock / 10000) - 1);
        //TIM7��ʼ������
   TIM_TimeBaseStructure.TIM_Period = 10*RS485_Frame_Distance; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
   TIM_TimeBaseStructure.TIM_Prescaler =Prescaler; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
   TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	// TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
 
         //TIM7�жϷ�������
   NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;  //TIM3�ж�
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
   NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���  
}

void TIM3_IRQHandler()
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		 TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//����жϱ�־
     TIM_Cmd(TIM3,DISABLE);//ֹͣ��ʱ��
     RS485_TX_EN=1;//ֹͣ���գ��л�Ϊ����״̬
     RS485_FrameFlag=1;//��λ֡�������
		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ

}