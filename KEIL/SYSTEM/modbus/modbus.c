#include "modbus.h"
#include "ADCDAC.h"
#include "oled.h"
#include "stm32f4xx.h"
#include "led.h"
/*
地址 功能码  按键1 按键2 LED1 LED2  ADC    DAC     CRC低位   CRC高位 

功能码0x02（读） 
 1  2              3                      4/5      6/7     8        9
       0000按键1 按键2 LED1 LED2         ADC       0   CRC低位   CRC高位  

功能码 0x05 （写）
1  2             3                     4/5      6/7     8       9
      000000  LED1 LED2                0       DAC  CRC低位   CRC高位

ADC，DAC扩大一千倍，写入寄存器。

从机（开发板）

 1       2                        3                   4/5       6/7      8         9
地址  字节数        0000按键1 按键2 LED1 LED2       ADC       0     CRC低位   CRC高位

*/


void GetSendbuff(u8* send_temp,u8 key1,u8 key2,u8 LED1,u8 LED2,float val,u8 ok) //ok借用byte3最高位
{
	u8 byte3=0x00;
	u16 adc_val=1000*val; //扩大一千倍
	send_temp[0]=mbaddress;
	send_temp[1]=1;
	byte3=byte3|key1;
	byte3=byte3<<1;
	byte3=byte3|key2;
	byte3=byte3<<1;
	byte3=byte3|LED1;
	byte3=byte3<<1;
	byte3=byte3|LED2;
	if(ok==1)
	{
		byte3=byte3|0x80;
	}
	send_temp[2]=byte3;
	
	send_temp[4]=adc_val;
  send_temp[3]=adc_val>>8;

  send_temp[5]=0;
  send_temp[6]=0;
  send_temp[7]=0;
  send_temp[8]=0;
	
	send_temp[7]=CalCRC(send_temp,7);
	send_temp[8]=CalCRC(send_temp,7)>>8;

}
/*
先进行CRC校验 然后比较地址随后进行操作
*/
int GetReceivebuff(u8 *buff,u8 *buff1)
{
	u8 i;
	u16 crc_get=0x00;
	u16 crc_test=CalCRC(buff,7);
	crc_get=(u16)(buff[8]<<8 | buff[7]);
	if((crc_get==crc_test)&&(buff[1]=mbaddress))
	{
		for(i=0;i<9;i++)
		{
			buff1[i]=buff[i];
		}
		return 1;
	}
	return 0;
}











