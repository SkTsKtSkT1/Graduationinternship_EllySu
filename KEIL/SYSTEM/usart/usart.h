#ifndef __USART_H_
#define __USART_H_
#include "stm32f4xx.h"
#include "stdio.h"	
#include "stm32f4xx_conf.h"

#define USART_REC_LEN 200
#define EN_USART6_RX 1

extern u8 USART_RX_BUF[USART_REC_LEN];
extern u16 USART_RX_STA; 

void uart_Init(u32 bound);

#endif 