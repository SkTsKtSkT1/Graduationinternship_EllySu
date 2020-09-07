#include "key.h"
#include "delay.h"
//KEY1: PE3
//KEY2: PE2

void Key_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOE,&GPIO_InitStructure);

}

u8 KEY_SCAN(u8 mode)
{
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0))
	{
		Delay_ms(10);//ȥ���� 
		key_up=0;
		 if(KEY1==0)return 1;
		else if(KEY2==0)return 2;
	}else if(KEY1==1&&KEY2==1)key_up=1; 	    
 	return 0;// �ް�������
}