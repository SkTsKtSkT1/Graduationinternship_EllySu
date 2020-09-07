#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	uart_Init(115200);
	u8 t;
	u8 len;
	u16 times=0;
	while(1)
	{
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
		}
	}*/
}

