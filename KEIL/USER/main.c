#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "beep.h"
#include "ADCDAC.h"
/*****
引脚分配:三路输出,三路输入:两路数字输入\输出,一路模拟输入\输出.
数字输入: KEY1 KEY2 
数字输出: LED1 LED2
模拟输入(ADC):  PF9  adc3 in7
模拟输出(DAC):  PA4
报警(蜂鸣器): PA8
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
			
		/*if(USART_RX_STA&0x8000) 接收到数据
		
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART6, USART_RX_BUF[t]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nQAQ\r\n");
			}
			if(times%200==0)printf("请输入数据,以回车键结束\r\n");  
			Delay_ms(10);   
		}*/
	}
}

