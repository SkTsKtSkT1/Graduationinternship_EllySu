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
		}
	}*/
}

