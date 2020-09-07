#ifndef __ADCDAC_H_
#define __ADCDAC_H_
#include "stm32f4xx.h"

/*****
引脚分配:三路输出,三路输入:两路数字输入\输出,一路模拟输入\输出.
数字输入: KEY1 KEY2
数字输出: LED1 LED2
模拟输入(ADC):  PF9
模拟输出(DAC):  PA4
***/
 void Dac_Init(void);
 void Adc_Init(void);
 void DAC_Set_Vol(u16 vol);
u16 GET_ADC_AVE(u8 ch,u8 times);
u16 Get_Adc(u8 ch);
#endif