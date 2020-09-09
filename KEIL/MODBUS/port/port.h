/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: port.h,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>
#include <inttypes.h>

#include "sys.h"

//#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   INTX_DISABLE()//__set_PRIMASK(1)  //�����ж�
#define EXIT_CRITICAL_SECTION( )    INTX_ENABLE()//__set_PRIMASK(0)  //�����ж�

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif


/******************************************************************************
                               ����Modbus��ز���
******************************************************************************/

#define REG_INPUT_START                          (USHORT)0x0001  //��ʼ�Ĵ���
#define REG_INPUT_NREGS                          (USHORT)4  //�Ĵ�������
#define REG_HOLDING_START                        (USHORT)0x0005  //���ּĴ���
#define REG_HOLDING_NREGS                        (USHORT)4  //���ּĴ�������

/******************************************************************************
                               �ⲿ���ù��ܺ���
******************************************************************************/

extern USHORT usRegInputBuf[REG_INPUT_NREGS];
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];


#endif