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
引脚分配:三路输出,三路输入:两路数字输入\输出,一路模拟输入\输出.
数字输入: KEY1 KEY2 
数字输出: LED1 LED2
模拟输入(ADC):  PF9  adc3 in7
模拟输出(DAC):  PA4
报警(蜂鸣器): PA8
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
	OLED_Refresh_Gram();//更新显示到OLED	 	
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
		OLED_ShowFloat(10,52,vol,12);	    
		OLED_Refresh_Gram();        //更新显示到OLED 
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

