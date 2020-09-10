#include "modbus.h"

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


void GetSendbuff(u8* send_temp,u8 key1,u8 key2,u8 LED1,u8 LED2,float val)
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

u8* GetReceivebuff(u8 *buff)
{
	
}