#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f4xx.h"

#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2

void Key_Init(void);
u8 KEY_SCAN(u8 mode);

#endif