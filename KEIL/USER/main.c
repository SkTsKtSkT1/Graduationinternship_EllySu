#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "beep.h"
#include "ADCDAC.h"
#include "oled.h"
#include "rs485.h"
#include "mbcrc.h"
#include "modbus.h"
#include "timer.h"
//168mHZ
int main()
{
	//RCC_ClocksTypeDef RCC_Clocks;
	//RCC_GetClocksFreq(&RCC_Clocks);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	Key_Init();
	OLED_Init();
	RS485_Init(9600);
	OLED_ShowString(0,0,"RemoteIo_ModeBus",16);
	OLED_Refresh_Gram( );//更新显示到OLED
//	LED1=0;
//	LED2=1;
	while(1)
	{
		RS485_service();
	}
}




/*


 void USART2_IRQHandler(void)//串口2中断服务程序
 {
       
        u8 res;
         u8 err;
      
       if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
         {
               if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//检测到噪音、帧错误或校验错误
                else err=0;
                //LED0=0;
                 res=USART_ReceiveData(USART2); //读接收到的字节，同时相关标志自动清除
                 
                 if((RS485_RX_CNT<2047)&&(err==0))
                {
                        RS485_RX_BUFF[RS485_RX_CNT]=res;
                         RS485_RX_CNT++;
                        
                        TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//清除定时器溢出中断
                        TIM_SetCounter(TIM7,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
                        TIM_Cmd(TIM7,ENABLE);//开始计时
                 }
         }
 }

///////////////////////////////////////////////////////////////////////////////////////
 //用定时器7判断接收空闲时间，当空闲时间大于指定时间，认为一帧结束
//定时器7中断服务程序         
 void TIM7_IRQHandler(void)
 {                                                                   
       if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
        {
                TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//清除中断标志
                TIM_Cmd(TIM7,DISABLE);//停止定时器
                RS485_TX_EN=1;//停止接收，切换为发送状态
                RS485_FrameFlag=1;//置位帧结束标记
         }
 }
 
 /////////////////////////////////////////////////////////////////////////////////////
//RS485服务程序，用于处理接收到的数据(请在主函数中循环调用)
u16 startRegAddr;
u16 RegNum;
u16 calCRC;
void RS485_Service(void)
{
        u16 recCRC;
        if(RS485_FrameFlag==1)
        {
                if(RS485_RX_BUFF[0]==RS485_Addr)//地址正确
                {
                        if((RS485_RX_BUFF[1]==01)||(RS485_RX_BUFF[1]==02)||(RS485_RX_BUFF[1]==03)||(RS485_RX_BUFF[1]==05)||(RS485_RX_BUFF[1]==06)||(RS485_RX_BUFF[1]==15)||(RS485_RX_BUFF[1]==16))//功能码正确
                  {
                                startRegAddr=(((u16)RS485_RX_BUFF[2])<<8)|RS485_RX_BUFF[3];//获取寄存器起始地址
                                if(startRegAddr<1000)//寄存器地址在范围内
                                {
                                        calCRC=CalCRC(RS485_RX_BUFF,RS485_RX_CNT-2);//计算所接收数据的CRC
                                        recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((u16)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//接收到的CRC(低字节在前，高字节在后)
                                        if(calCRC==recCRC)//CRC校验正确
                                        {
                                                ///////////显示用

        
///////////////////////
                                                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                switch(RS485_RX_BUFF[1])//根据不同的功能码进行处理
                                                {
                                                        case 2://读输入开关量
                                                        {
                                                                Modbus_02_Solve();
                                                                break;
                                                        }

                                                        case 1://读输出开关量
                                                        {
                                                                Modbus_01_Solve();
                                                                break;
                                                        }

                                                        case 5://写单个输出开关量
                                                        {
                                                                Modbus_05_Solve();
                                                                break;
                                                        }

                                                        case 15://写多个输出开关量
                                                        {
                                                                Modbus_15_Solve();
                                                                break;
                                                        }

                                                        case 03: //读多个寄存器
                                                        {
                                                                Modbus_03_Solve();
                                                                break;
                                                        }

                                                        case 06: //写单个寄存器
                                                        {
                                                                Modbus_06_Solve();
                                                                break;
                                                        }

                                                        case 16: //写多个寄存器
                                                        {
                                                                Modbus_16_Solve();
                                                                break;
                                                        }


                                                }
                                                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        }
                                        else//CRC校验错误
                                        {

                                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                                RS485_TX_BUFF[2]=0x04; //异常码
                                                RS485_SendData(RS485_TX_BUFF,3);
                                        }
                                }
                                else//寄存器地址超出范围
                                {
                                        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                        RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                        RS485_TX_BUFF[2]=0x02; //异常码
                                        RS485_SendData(RS485_TX_BUFF,3);
                                }
                        }
                        else//功能码错误
                        {
                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                RS485_TX_BUFF[2]=0x01; //异常码
                                RS485_SendData(RS485_TX_BUFF,3);
                        }
          }

                RS485_FrameFlag=0;//复位帧结束标志
                RS485_RX_CNT=0;//接收计数器清零
                RS485_TX_EN=0;//开启接收模式
        }
}


int main()
{

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	//uart_Init(115200);
	Key_Init();
	//Beep_Init();
	//Dac_Init();
	//Adc_Init();
	OLED_Init();
	RS485_Init();
	OLED_ShowString(0,0,"RemoteIo_ModeBus",16);  
	OLED_Refresh_Gram( );//更新显示到OLED	
	while(1)
	{
		RS485_Service();
	}

}
*/