#include "modbus.h"
#include "ADCDAC.h"
#include "oled.h"
#include "stm32f4xx.h"
#include "led.h"
#include "rs485.h"
/*
 * <------------------------ MODBUS SERIAL LINE PDU (1) ------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+----------------------------+-------------+
 *  | Address   | Function Code | Data                       | CRC/LRC     |
 *  +-----------+---------------+----------------------------+-------------+
 *  |           |               |                                   |
 * (2)        (3/2')           (3')                                (4)
 *
 * (1)  ... MB_SER_PDU_SIZE_MAX = 256
 * (2)  ... MB_SER_PDU_ADDR_OFF = 0
 * (3)  ... MB_SER_PDU_PDU_OFF  = 1
 * (4)  ... MB_SER_PDU_SIZE_CRC = 2
 *
 * (1') ... MB_PDU_SIZE_MAX     = 253
 * (2') ... MB_PDU_FUNC_OFF     = 0
 * (3') ... MB_PDU_DATA_OFF     = 1
 */
 u32 testData1=0,testData2=2000,testData3=00,testData4=0;
 vu32 *Modbus_InputIO[100]={0};
 vu32 *Modbus_OutputIO[100]={0};
 u16  *Modbus_HoldReg[1000]={0};
 
 u16 startRegAddr;
 u16 RegNum;
 u16 CRC_Cal;
 u8 RS485_Addr=0x01;
 u8 RS485_FrameFlag=0;//帧结束标记
 u8 RS485_TX_BUFF[256];
 
 u16 dac_vol=0;
 u16 adc_vol=0;
 
void Modbus_RegMap(void)
{
	//输入开关量
	   Modbus_InputIO[0]=(vu32*)&PEin(3);//KEY1
	   Modbus_InputIO[1]=(vu32*)&PEin(2);//KEY2
	//输出开关量
	   Modbus_OutputIO[0]=(vu32*)&PBout(12);//LED1
	   Modbus_OutputIO[1]=(vu32*)&PBout(13);//LED2
		   Modbus_OutputIO[2]=(vu32*)&PBout(14);//LED3
	   Modbus_OutputIO[3]=(vu32*)&PBout(15);//LED4
		   Modbus_OutputIO[4]=(vu32*)&PBout(11);//LED5
	
	//寄存器测试
		 Modbus_HoldReg[0]=(u16*)&testData1; //ADC
		 Modbus_HoldReg[1]=(u16*)&testData2; //DAC
		 Modbus_HoldReg[2]=(u16*)&testData3;
		 Modbus_HoldReg[3]=(u16*)&testData1;
		 Modbus_HoldReg[4]=(u16*)&testData2;
		 Modbus_HoldReg[5]=(u16*)&testData2;
}

void Modbus_01_Solve()// LED为低位点亮 因此两个都不亮->0X03
{
	u16 Bytenum;
	u16 i;
	RegNum=((((u16)RS485_RX_BUF[4])<<8)|(RS485_RX_BUF[5]));
	if((startRegAddr+RegNum)<100)
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		Bytenum=RegNum/8;
		if(RegNum%8) Bytenum+=1;
		RS485_TX_BUFF[2]=Bytenum;
		for(i=0;i<=Bytenum;i++)  //比较可能出问题的地方
		{
			if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
			RS485_TX_BUFF[3+i/8]>>=1;
			RS485_TX_BUFF[3+i/8]|=((*Modbus_OutputIO[startRegAddr+i]<<7)&0x80);
			if(i==RegNum-1)
			{
				if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//add zero;
			}
		}
		CRC_Cal=CalCRC(RS485_TX_BUFF,Bytenum+3);
		RS485_TX_BUFF[Bytenum+3]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[Bytenum+4]=(CRC_Cal>>8)&0xFF;  //low high
		RS485_Send_Data(RS485_TX_BUFF,Bytenum+5);
		
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1]|0x80;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
		
	}
}

void Modbus_02_Solve()// KEY按下为低电平,因此若无按键则为0X03
{
	u16 Bytenum;
	u16 i;
	RegNum=((((u16)RS485_RX_BUF[4])<<8)|(RS485_RX_BUF[5]));
	if((startRegAddr+RegNum)<100)
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		Bytenum=RegNum/8;
		if(RegNum%8) Bytenum+=1;
		RS485_TX_BUFF[2]=Bytenum;
		for(i=0;i<=Bytenum;i++) //比较可能出问题的地方
		{
			if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
			RS485_TX_BUFF[3+i/8]>>=1;
			RS485_TX_BUFF[3+i/8]|=((*Modbus_InputIO[startRegAddr+i]<<7)&0x80);
			if(i==RegNum-1)
			{
				if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//add zero;
			}
		}
		CRC_Cal=CalCRC(RS485_TX_BUFF,Bytenum+3);
		RS485_TX_BUFF[Bytenum+3]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[Bytenum+4]=(CRC_Cal>>8)&0xFF;  //low high
		RS485_Send_Data(RS485_TX_BUFF,Bytenum+5);
		
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=0x82;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
	}
}

void Modbus_03_Solve()
{
	u16 i;
	RegNum=((((u16)RS485_RX_BUF[4])<<8)|(RS485_RX_BUF[5]));
	if((startRegAddr+RegNum)<1000)
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		RS485_TX_BUFF[2]=RegNum*2;
		
		for(i=0;i<RegNum;i++)
		{
			RS485_TX_BUFF[3+i*2]=(*Modbus_HoldReg[startRegAddr+i]>>8)&0xFF;
			RS485_TX_BUFF[4+i*2]=(*Modbus_HoldReg[startRegAddr+i])&0xFF;
		}
		CRC_Cal=CalCRC(RS485_TX_BUFF,RegNum*2+3);
		RS485_TX_BUFF[RegNum*2+3]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[RegNum*2+4]=(CRC_Cal>>8)&0xFF;  //low high
		RS485_Send_Data(RS485_TX_BUFF,RegNum*2+5);
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=0x83;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
	}
}

void Modbus_05_Solve()  //LED是反的
{
	if((startRegAddr+RegNum)<100)
	{
		if((RS485_RX_BUF[4]==0xFF)&&(RS485_RX_BUF[5]==0x00)) *Modbus_OutputIO[startRegAddr]=0x00;
		if((RS485_RX_BUF[4]==0x00)&&(RS485_RX_BUF[5]==0x00)) *Modbus_OutputIO[startRegAddr]=0x01;
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		RS485_TX_BUFF[2]=RS485_RX_BUF[2];
		RS485_TX_BUFF[3]=RS485_RX_BUF[3];		
		RS485_TX_BUFF[4]=RS485_RX_BUF[4];
		RS485_TX_BUFF[5]=RS485_RX_BUF[5];	
		
		CRC_Cal=CalCRC(RS485_TX_BUFF,6);
		RS485_TX_BUFF[6]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[7]=(CRC_Cal>>8)&0xFF;  //low high		
		RS485_Send_Data(RS485_TX_BUFF,8);
		
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=0x85;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
	}
}

void Modbus_06_Solve()
{
	if(startRegAddr<1000)
	{
		*Modbus_HoldReg[startRegAddr]=RS485_RX_BUF[4]<<8;
		*Modbus_HoldReg[startRegAddr]|=((u16)RS485_RX_BUF[5]);
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		RS485_TX_BUFF[2]=RS485_RX_BUF[2];
		RS485_TX_BUFF[3]=RS485_RX_BUF[3];		
		RS485_TX_BUFF[4]=RS485_RX_BUF[4];
		RS485_TX_BUFF[5]=RS485_RX_BUF[5];	
		
		CRC_Cal=CalCRC(RS485_TX_BUFF,6);
		RS485_TX_BUFF[6]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[7]=(CRC_Cal>>8)&0xFF;  //low high		
		RS485_Send_Data(RS485_TX_BUFF,8);
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=0x86;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
	}
}

void Modbus_0f_Solve() //LED 反的 RX[7+i] i=0~k,先写低地址
{
	u16 i;
	RegNum=((((u16)RS485_RX_BUF[4])<<8)|(RS485_RX_BUF[5]));
	
	if((startRegAddr+RegNum)<100)
	{
		for(i=0;i<RegNum;i++)
		{
			if(RS485_RX_BUF[7+i/8]&0x01) *Modbus_OutputIO[startRegAddr+i]=0x00;
			else *Modbus_OutputIO[startRegAddr+i]=0x01;
			RS485_RX_BUF[7+i/8]>>=1;
		}
	
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		RS485_TX_BUFF[2]=RS485_RX_BUF[2];
		RS485_TX_BUFF[3]=RS485_RX_BUF[3];		
		RS485_TX_BUFF[4]=RS485_RX_BUF[4];
		RS485_TX_BUFF[5]=RS485_RX_BUF[5];
		CRC_Cal=CalCRC(RS485_TX_BUFF,6);
		RS485_TX_BUFF[6]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[7]=(CRC_Cal>>8)&0xFF;  //low high		
		RS485_Send_Data(RS485_TX_BUFF,8);		
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=0x8F;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
	}
	
	
}

void Modbus_10_Solve()
{
	u16 i;
	RegNum=((((u16)RS485_RX_BUF[4])<<8)|(RS485_RX_BUF[5]));
	if(startRegAddr+RegNum<1000)
	{
		for(i=0;i<RegNum;i++)
		{
			*Modbus_HoldReg[startRegAddr+i]=RS485_RX_BUF[7+i*2]<<8;
			*Modbus_HoldReg[startRegAddr+i]|=((u16)RS485_RX_BUF[8+i*2]);
		}
		
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=RS485_RX_BUF[1];
		RS485_TX_BUFF[2]=RS485_RX_BUF[2];
		RS485_TX_BUFF[3]=RS485_RX_BUF[3];		
		RS485_TX_BUFF[4]=RS485_RX_BUF[4];
		RS485_TX_BUFF[5]=RS485_RX_BUF[5];	
		
		CRC_Cal=CalCRC(RS485_TX_BUFF,6);
		RS485_TX_BUFF[6]=(CRC_Cal)&0xFF;
		RS485_TX_BUFF[7]=(CRC_Cal>>8)&0xFF;  //low high		
		RS485_Send_Data(RS485_TX_BUFF,8);
	}
	else
	{
		RS485_TX_BUFF[0]=RS485_RX_BUF[0];
		RS485_TX_BUFF[1]=0x90;
		RS485_TX_BUFF[2]=0x02;
		RS485_Send_Data(RS485_TX_BUFF,3);
	}
}
void RS485_service()
{
	u16 CRC_Rec;
	adc_vol=Get_Adc(8);
	testData1=adc_vol;   //ADC_VOL/4096*3.3=VOL
	if((testData2<3300)&&(testData2>0))
	{
		dac_vol=testData2;
		DAC_Set_Vol(dac_vol);   //VOL=DAC_VOL/3300*3.3
	}
	OLED_ShowString(0,20,"AI:",12);OLED_ShowNum(20,20,adc_vol,8,12);
	OLED_ShowString(0,40,"AO:",12);OLED_ShowNum(40,20,dac_vol,8,12);
	OLED_Refresh_Gram( );//更新显示到OLED
	if(RS485_FrameFlag==1)
	{
		if(RS485_RX_BUF[0]==RS485_Addr)//地址正确
		{
			if((RS485_RX_BUF[1]==0x01)||(RS485_RX_BUF[1]==0x02)||(RS485_RX_BUF[1]==0x03)||(RS485_RX_BUF[1]==0x05)||(RS485_RX_BUF[1]==0x06)||(RS485_RX_BUF[1]==0x0f)||(RS485_RX_BUF[1]==0x10))
			{
				startRegAddr=((u16)RS485_RX_BUF[2]<<8)|RS485_RX_BUF[3];//获得起始地址
				if(startRegAddr<1000) //地址正常
				{
					CRC_Cal=CalCRC(RS485_RX_BUF,RS485_RX_CNT-2);
					CRC_Rec=((u16)RS485_RX_BUF[RS485_RX_CNT-1]<<8)|RS485_RX_BUF[RS485_RX_CNT-2] ;  //crc low crc high
					if(CRC_Cal==CRC_Rec)
					{
						switch(RS485_RX_BUF[1])
						{
							case 0x01:
							{
								Modbus_01_Solve();
								break;
							}
							case 0x02:
							{
								Modbus_02_Solve();
								break;
							}
							case 0x03:
							{
								Modbus_03_Solve();
								break;
							}
							case 0x05:
							{
								Modbus_05_Solve();
								break;
							}
							case 0x06:
							{
								Modbus_06_Solve();
								break;
							}
							case 0x0f:
							{
								
								Modbus_0f_Solve();
								break;
							}
							case 0x10:
							{
								Modbus_10_Solve();
								break;
							}
						}
					}
					else //crc err
					{
					 RS485_TX_BUFF[0]=RS485_RX_BUF[0];
					 RS485_TX_BUFF[1]=RS485_RX_BUF[1]|0x80;
           RS485_TX_BUFF[2]=0x04; //异常码
           RS485_Send_Data(RS485_TX_BUFF,3);
					}
				}
				else //crc err
				{
					 RS485_TX_BUFF[0]=RS485_RX_BUF[0];
					 RS485_TX_BUFF[1]=RS485_RX_BUF[1]|0x80;
           RS485_TX_BUFF[2]=0x02; //异常码
           RS485_Send_Data(RS485_TX_BUFF,3);
				}
			}
			else //func code err
			{
				RS485_TX_BUFF[0]=RS485_RX_BUF[0];
				RS485_TX_BUFF[1]=RS485_RX_BUF[1]|0x80;
        RS485_TX_BUFF[2]=0x01; //异常码
        RS485_Send_Data(RS485_TX_BUFF,3);
			}
		}
		/*end*/
		 RS485_FrameFlag=0;//复位帧结束标志
     RS485_RX_CNT=0;//接收计数器清零
     RS485_TX_EN=0;//开启接收模式  
	}
	
}





