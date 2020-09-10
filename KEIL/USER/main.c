#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "beep.h"
#include "ADCDAC.h"
#include "oled.h"
#include "rs485.h"
#include "mbcrc.h"
#include "modbus.h"
/*****
引脚分配:三路输出,三路输入:两路数字输入\输出,一路模拟输入\输出.
数字输入: KEY1 KEY2 
数字输出: LED1 LED2
模拟输入(ADC):  PF9  adc3 in7
模拟输出(DAC):  PA4
报警(蜂鸣器): PA8
***/
 /*****
 功能码：
 *****/
 u8 LED1=0,LED2=0;
void open_led(u16 led_pin)
{
	GPIO_ResetBits(GPIOB,LED1_PIN);
	switch(led_pin)
	{
		case LED1_PIN:LED1=1;break;
		case LED2_PIN:LED2=1;break;
	}
}
void close_led(u16 led_pin)
{
	GPIO_SetBits(GPIOB,LED1_PIN);
	switch(led_pin)
	{
		case LED1_PIN:LED1=0;break;
		case LED2_PIN:LED2=0;break;
	}
}

int main()
{
	float vol;
	u8 key,key1,key2;
	u8 rs485_i;
	u8 rs485_len=0;
	RS485_TX_EN=0;
	u8 rs485Sendbuf[9]={0};
	u8 rs485Receivebuf[9]={0};
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
	open_led(LED1_PIN);	
	while(1)
	{
		key=KEY_SCAN(0);
		vol=(float)Get_Adc(7)*(3.3/4096);
		switch(key)
		{
			case 1:OLED_ShowString(15,24,"Key 1 is on",12);key1=1;key2=0;break;
			case 2:OLED_ShowString(15,24,"Key 2 is on",12);key2=1;key1=0;break;
			default:OLED_ShowString(15,24,"                ",12);key1=0;key2=0;   //按钮的状态如何被保存住，想法：删掉default 然后在第一句清零
		}
		OLED_ShowFloat(10,52,vol,12);	    
		OLED_Refresh_Gram();        //更新显示到OLED 
		if(! RS485_TX_EN)
		{
			RS485_Receive_Data(rs485Receivebuf,&rs485_len);
		}
		if(rs485_len)
		{
			if(rs485_len>9) rs485_len=9;
			if(rs485Receivebuf[0]==0x01)
			{
				
        RS485_TX_EN=1;
			}
		}
		if(RS485_TX_EN)
		{
			GetSendbuff(rs485Sendbuf,key1,key2,LED1,LED2,vol);
			RS485_Send_Data(rs485Sendbuf,9);
			Delay_ms(10);
			RS485_TX_EN=0;
		}
		
		Delay_ms(500);
	}

}

