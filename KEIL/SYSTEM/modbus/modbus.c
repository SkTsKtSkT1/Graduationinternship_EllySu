#include "modbus.h"

/*
��ַ ������  ����1 ����2 LED1 LED2  ADC    DAC     CRC��λ   CRC��λ 

������0x02������ 
 1  2              3                      4/5      6/7     8        9
       0000����1 ����2 LED1 LED2         ADC       0   CRC��λ   CRC��λ  

������ 0x05 ��д��
1  2             3                     4/5      6/7     8       9
      000000  LED1 LED2                0       DAC  CRC��λ   CRC��λ

ADC��DAC����һǧ����д��Ĵ�����

�ӻ��������壩

 1       2                        3                   4/5       6/7      8         9
��ַ  �ֽ���        0000����1 ����2 LED1 LED2       ADC       0     CRC��λ   CRC��λ

*/


void GetSendbuff(u8* send_temp,u8 key1,u8 key2,u8 LED1,u8 LED2,float val)
{
	u8 byte3=0x00;
	u16 adc_val=1000*val; //����һǧ��
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