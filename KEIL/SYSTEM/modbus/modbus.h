#ifndef __MODBUS_H_
#define __MODBUS_H_

#include "mbcrc.h"
#include "sys.h"

extern u16 startRegAddr;
extern u16 RegNum;
extern u16 CRC_Cal;

//²âÊÔ¼Ä´æÆ÷
extern u32 testData1,testData2,testData3;

extern vu32 *Modbus_InputIO[100];
extern vu32 *Modbus_OutputIO[100];
extern u16  *Modbus_HoldReg[1000];



void Modbus_RegMap(void);
void Modbus_01_Solve(void);
void Modbus_02_Solve(void);
void Modbus_03_Solve(void);
void Modbus_05_Solve(void);
void Modbus_06_Solve(void);
void Modbus_0f_Solve(void);
void Modbus_10_Solve(void);
void RS485_service(void);
#endif