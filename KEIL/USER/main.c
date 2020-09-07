#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "beep.h"
#include "ADCDAC.h"
/*****
���ŷ���:��·���,��·����:��·��������\���,һ·ģ������\���.
��������: KEY1 KEY2 
�������: LED1 LED2
ģ������(ADC):  PF9  adc3 in7
ģ�����(DAC):  PA4
����(������): PA8
***/


int main()
{
	//float vol;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	uart_Init(115200);
	Key_Init();
	Beep_Init();
	Dac_Init();
	Adc_Init();
	while(1)
	{
		//vol=(float)Get_Adc(7)*(3.3/4096);
		//printf("ADC is:%fV\r\n",vol);
		//Delay_ms(100);
	  DAC_Set_Vol(2000);
		//Delay_ms(100);
			
		/*if(USART_RX_STA&0x8000) ���յ�����
		
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART6, USART_RX_BUF[t]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nQAQ\r\n");
			}
			if(times%200==0)printf("����������,�Իس�������\r\n");  
			Delay_ms(10);   
		}*/
	}
}

