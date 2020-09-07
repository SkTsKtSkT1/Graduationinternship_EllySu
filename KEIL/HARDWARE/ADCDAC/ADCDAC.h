#ifndef __ADCDAC_H_
#define __ADCDAC_H_
#include "stm32f4xx.h"

/*****
���ŷ���:��·���,��·����:��·��������\���,һ·ģ������\���.
��������: KEY1 KEY2
�������: LED1 LED2
ģ������(ADC):  PF9
ģ�����(DAC):  PA4
***/
 void Dac_Init(void);
 void Adc_Init(void);
 void DAC_Set_Vol(u16 vol);
u16 GET_ADC_AVE(u8 ch,u8 times);
u16 Get_Adc(u8 ch);
#endif