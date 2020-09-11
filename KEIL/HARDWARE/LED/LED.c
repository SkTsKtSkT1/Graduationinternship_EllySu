#include "LED.h"
 #include "stm32f4xx.h"
 

 
 
 void LED_Init(void)
 {
	  GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	  GPIO_InitStructure.GPIO_Pin=ALL_LED;
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
	 
		GPIO_SetBits(GPIOB,ALL_LED);
 }
 
