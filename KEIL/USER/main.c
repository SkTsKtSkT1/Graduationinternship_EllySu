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
 /*
地址 功能码  按键1 按键2 LED1 LED2  ADC    DAC     CRC低位   CRC高位 

功能码0x02（读）  0
 1  2              3                      4/5      6/7     8        9
       0000按键1 按键2 LED1 LED2         ADC       0   CRC低位   CRC高位  

功能码 0x05 （写） 1
1  2             3                     4/5      6/7     8       9
      000000  LED1 LED2                0        DAC  CRC低位   CRC高位

ADC，DAC扩大一千倍，写入寄存器。
*/
u8 LED1=0,LED2=0;
 void open_led(u16 led_pin)
{
	GPIO_ResetBits(GPIOB,led_pin);
	switch(led_pin)
	{
		case LED1_PIN:LED1=1;break;
		case LED2_PIN:LED2=1;break;
	}
}
void close_led(u16 led_pin)
{
	GPIO_SetBits(GPIOB,led_pin);
	switch(led_pin)
	{
		case LED1_PIN:LED1=0;break;
		case LED2_PIN:LED2=0;break;
	}
}

int main()
{
	float vol,DACvol;
	u16 DACVol=0x00;
	u16 crc_get=0x00;
	u16 crc_test;
	u8 key=0,key1=0,key2=0;
	u8 crc_flag=0;
	u8 rs485_i=0;
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
	OLED_ShowString(0,24,"DI:",12);  
 	OLED_ShowString(0,40,"DO:",12);  
 	OLED_ShowString(0,52,"AI:",12);  
 	OLED_ShowString(64,52,"AO:",12); 
	OLED_ShowString(80,52,"...",12);	
	OLED_Refresh_Gram( );//更新显示到OLED
	close_led(LED1_PIN);	
	close_led(LED2_PIN);
	DAC_Set_Vol(0);
	while(1)
	{
		key=KEY_SCAN(0);
		vol=(float)GET_ADC_AVE(8,5)*(3.3/4096);
		/*test*/
		//DACVol=(DACVol|buf[5])<<8;
		//DACVol=(DACVol|buf[6]);
		//DAC_Set_Vol((int)DACVol);
		//OLED_ShowNum(0,40,DACvol,8,12);
		//DACvol=(float)(DACVol/3300)*3.3;
		
		//OLED_ShowFloat(70,52,DACvol,12);	
		/**/
		OLED_ShowFloat(10,52,vol,12);	    
		OLED_Refresh_Gram();        //更新显示到OLED
		switch(key)
		{
			case 1:OLED_ShowString(15,24,"Key 1 is on",12);key1=1;key2=0;break;
			case 2:OLED_ShowString(15,24,"Key 2 is on",12);key2=1;key1=0;break;
			default:OLED_ShowString(15,24,"                ",12);key1=0;key2=0;   //按钮的状态如何被保存住，想法：删掉default 然后在第一句清零
		}
 
		if(! RS485_TX_EN)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
		{
			RS485_Receive_Data(rs485Receivebuf,&rs485_len);
			//recflag=GetReceivebuff(rs485Receivebuf,Rec_Temp);
			crc_test=CalCRC(rs485Receivebuf,7);
			crc_get=(u16)(rs485Receivebuf[8]<<8 |rs485Receivebuf[7]);
			//OLED_ShowNum(0,20,crc_test,16,12);
			//OLED_ShowNum(0,40,crc_get,16,12);
			//OLED_Refresh_Gram();//更新显示到OLED
			if((crc_test==crc_get)&&(rs485Receivebuf[1]==mbaddress))
			{
			crc_flag=1;
			//OLED_ShowNum(10,50,crc_flag,3,12);
			//OLED_Refresh_Gram();//更新显示到OLED
			}
			else crc_flag=0;
			Delay_ms(10);
			
		}
		if(rs485_len&&crc_flag)
		{
			if(rs485_len>9) rs485_len=9;
			if(rs485Receivebuf[0]==0x02)
			{
				GetSendbuff(rs485Sendbuf,key1,key2,LED1,LED2,vol,0);
			}
			if(rs485Receivebuf[0]==0x05) //发送成功后返回一帧
			{
				Delay_ms(10);
				switch(rs485Receivebuf[2]) //
				{
					case 0x02:open_led(LED1_PIN);close_led(LED2_PIN);break;
					case 0x01:close_led(LED1_PIN);open_led(LED2_PIN);break;
					case 0x00:close_led(LED1_PIN);close_led(LED2_PIN);break;
					case 0x03:open_led(LED1_PIN);open_led(LED2_PIN); break;
					default: break;
				}
				Delay_ms(10);
				DACVol=(DACVol|rs485Receivebuf[5])<<8;
				printf("Vol:%X,rs485Sendbuf[5]:%X\r\n",DACVol,rs485Receivebuf[5]);
				DACVol=(DACVol|rs485Receivebuf[6]);
				printf("Vol:%X,rs485Sendbuf[6]:%X\r\n",DACVol,rs485Receivebuf[6]);
				Delay_ms(10);
				DAC_Set_Vol((int)DACVol);
				DACvol=(float)DACVol/3300*3.3;
				printf("Vol:%1.3f\r\n",DACvol);
				OLED_ShowFloat(70,52,DACvol,12);	    
				OLED_Refresh_Gram();        //更新显示到OLED
				GetSendbuff(rs485Sendbuf,key1,key2,LED1,LED2,vol,1);
			}
				RS485_TX_EN=1;
				DACVol=0x00;
			  for(rs485_i=0;rs485_i<9;rs485_i++)
				{
					rs485Receivebuf[rs485_i]=0;
				}
		}
		if(RS485_TX_EN)
		{
			RS485_Send_Data(rs485Sendbuf,9);
			Delay_ms(10);
			RS485_TX_EN=0;
			for(rs485_i=0;rs485_i<9;rs485_i++)
			{
				rs485Sendbuf[rs485_i]=0;
			}
		}
		
		Delay_ms(500);
	}

}

