#ifndef __MODBUS_H_
#define __MODBUS_H_

#include "mbcrc.h"
#include "sys.h"

#define mbaddress 0x00
/*
��ַ ������  ����1 ����2 LED1 LED2  ADC    DAC     CRC��λ   CRC��λ 

������0x02������ 
 1  2             3                      4          5      6     7
      0000����1 ����2 LED1 LED2         ADC       0  CRC��λ   CRC��λ

������ 0x05 ��д��
1  2             3                     4        5      6     7
      000000  LED1 LED2                0       DAC  CRC��λ   CRC��λ

ADC��DAC����λΪ�������֣�����λΪС����������λ����000���� xxx0

*/
//u8 receive_temp[7]={0};

void GetSendbuff(u8* send_temp,u8 key1,u8 key2,u8 LED1,u8 LED2,float val);

u8* GetReceivebuff(u8 *buff);

#endif