#ifndef __MODBUS_H_
#define __MODBUS_H_

#include "mbcrc.h"
#include "sys.h"

#define mbaddress 0x00
/*
地址 功能码  按键1 按键2 LED1 LED2  ADC    DAC     CRC低位   CRC高位 

功能码0x02（读） 
 1  2             3                      4          5      6     7
      0000按键1 按键2 LED1 LED2         ADC       0  CRC低位   CRC高位

功能码 0x05 （写）
1  2             3                     4        5      6     7
      000000  LED1 LED2                0       DAC  CRC低位   CRC高位

ADC，DAC高四位为整数部分，低四位为小数（保留三位）：000整数 xxx0

*/
//u8 receive_temp[7]={0};

void GetSendbuff(u8* send_temp,u8 key1,u8 key2,u8 LED1,u8 LED2,float val);

u8* GetReceivebuff(u8 *buff);

#endif