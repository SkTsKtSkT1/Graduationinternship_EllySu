#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "beep.h"
#include "ADCDAC.h"
#include "oled.h"
#include "rs485.h"
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
	float vol;
	u8 key;
	u8 rs485_i;
	u8 rs485_len=0;
	RS485_TX_EN=0;
	u8 rs485Sendbuf[8]={};
	u8 rs485Receivebuf[8]={0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	uart_Init(115200);
	Key_Init();
	Beep_Init();
	Dac_Init();
	Adc_Init();
	OLED_Init();
	RS485_Init(9600);
	OLED_ShowString(0,0,"RemoteIo_ModeBus",16);  
	OLED_ShowString(0,24, "DI:",12);  
 	OLED_ShowString(0,40,"DO:",12);  
 	OLED_ShowString(0,52,"AI:",12);  
 	OLED_ShowString(64,52,"AO",12);  
	OLED_Refresh_Gram();//������ʾ��OLED	 	
	while(1)
	{
		key=KEY_SCAN(0);
		vol=(float)Get_Adc(7)*(3.3/4096);
		switch(key)
		{
			case 1:OLED_ShowString(15,24,"Key 1 is on",12);break;
			case 2:OLED_ShowString(15,24,"Key 2 is on",12);break;
			default:OLED_ShowString(15,24,"                ",12);
		}
		//printf("ADC is:%fV\r\n",vol);			
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
		OLED_ShowFloat(10,52,vol,12);	    
		OLED_Refresh_Gram();        //������ʾ��OLED 
		if(! RS485_TX_EN)
		{
			RS485_Receive_Data(rs485Receivebuf,&rs485_len);
		}
		if(rs485_len)
		{
			if(rs485_len>5) rs485_len=5;
			if(rs485Receivebuf[0]==0x01)
			{
				GPIO_ResetBits(GPIOB,LED1_PIN);
        RS485_TX_EN=1;
			}
		}
		if(RS485_TX_EN)
		{
			RS485_Send_Data(rs485Sendbuf,5);
			GPIO_SetBits(GPIOB,LED1_PIN);
			Delay_ms(10);
			RS485_TX_EN=0;
		}
		Delay_ms(500);
	}

}

