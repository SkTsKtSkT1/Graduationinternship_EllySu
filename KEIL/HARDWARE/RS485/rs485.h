#ifndef __RS485_H
#define __RS485_H			 
#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//RS485���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
	  	
extern u8 RS485_RX_BUF[256]; 		//���ջ���,���2048���ֽ�
extern u16 RS485_RX_CNT;   			//���յ������ݳ���

extern u8 RS485_TX_BUFF[256];//���ͻ�����
extern u16 RS485_TX_CNT;

extern u8 RS485_Addr;
extern u32 RS485_Boundrate;
extern u16 RS485_Frame_Distance;  //��λ ms
extern u8 RS485_FrameFlag;//֡�������

//ģʽ����
#define RS485_TX_EN		PGout(8)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ�����EN_USART2_RXΪ1����������Ϊ0
#define EN_USART2_RX 	1			//0,������;1,����.

														 
void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);		 
#endif	   
















