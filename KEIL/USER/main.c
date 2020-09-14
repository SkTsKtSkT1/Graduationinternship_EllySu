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
	OLED_Refresh_Gram( );//������ʾ��OLED
//	LED1=0;
//	LED2=1;
	while(1)
	{
		RS485_service();
	}
}




/*


 void USART2_IRQHandler(void)//����2�жϷ������
 {
       
        u8 res;
         u8 err;
      
       if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
         {
               if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//��⵽������֡�����У�����
                else err=0;
                //LED0=0;
                 res=USART_ReceiveData(USART2); //�����յ����ֽڣ�ͬʱ��ر�־�Զ����
                 
                 if((RS485_RX_CNT<2047)&&(err==0))
                {
                        RS485_RX_BUFF[RS485_RX_CNT]=res;
                         RS485_RX_CNT++;
                        
                        TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//�����ʱ������ж�
                        TIM_SetCounter(TIM7,0);//�����յ�һ���µ��ֽڣ�����ʱ��7��λΪ0�����¼�ʱ���൱��ι����
                        TIM_Cmd(TIM7,ENABLE);//��ʼ��ʱ
                 }
         }
 }

///////////////////////////////////////////////////////////////////////////////////////
 //�ö�ʱ��7�жϽ��տ���ʱ�䣬������ʱ�����ָ��ʱ�䣬��Ϊһ֡����
//��ʱ��7�жϷ������         
 void TIM7_IRQHandler(void)
 {                                                                   
       if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
        {
                TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//����жϱ�־
                TIM_Cmd(TIM7,DISABLE);//ֹͣ��ʱ��
                RS485_TX_EN=1;//ֹͣ���գ��л�Ϊ����״̬
                RS485_FrameFlag=1;//��λ֡�������
         }
 }
 
 /////////////////////////////////////////////////////////////////////////////////////
//RS485����������ڴ�����յ�������(������������ѭ������)
u16 startRegAddr;
u16 RegNum;
u16 calCRC;
void RS485_Service(void)
{
        u16 recCRC;
        if(RS485_FrameFlag==1)
        {
                if(RS485_RX_BUFF[0]==RS485_Addr)//��ַ��ȷ
                {
                        if((RS485_RX_BUFF[1]==01)||(RS485_RX_BUFF[1]==02)||(RS485_RX_BUFF[1]==03)||(RS485_RX_BUFF[1]==05)||(RS485_RX_BUFF[1]==06)||(RS485_RX_BUFF[1]==15)||(RS485_RX_BUFF[1]==16))//��������ȷ
                  {
                                startRegAddr=(((u16)RS485_RX_BUFF[2])<<8)|RS485_RX_BUFF[3];//��ȡ�Ĵ�����ʼ��ַ
                                if(startRegAddr<1000)//�Ĵ�����ַ�ڷ�Χ��
                                {
                                        calCRC=CalCRC(RS485_RX_BUFF,RS485_RX_CNT-2);//�������������ݵ�CRC
                                        recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((u16)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//���յ���CRC(���ֽ���ǰ�����ֽ��ں�)
                                        if(calCRC==recCRC)//CRCУ����ȷ
                                        {
                                                ///////////��ʾ��

        
///////////////////////
                                                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                switch(RS485_RX_BUFF[1])//���ݲ�ͬ�Ĺ�������д���
                                                {
                                                        case 2://�����뿪����
                                                        {
                                                                Modbus_02_Solve();
                                                                break;
                                                        }

                                                        case 1://�����������
                                                        {
                                                                Modbus_01_Solve();
                                                                break;
                                                        }

                                                        case 5://д�������������
                                                        {
                                                                Modbus_05_Solve();
                                                                break;
                                                        }

                                                        case 15://д������������
                                                        {
                                                                Modbus_15_Solve();
                                                                break;
                                                        }

                                                        case 03: //������Ĵ���
                                                        {
                                                                Modbus_03_Solve();
                                                                break;
                                                        }

                                                        case 06: //д�����Ĵ���
                                                        {
                                                                Modbus_06_Solve();
                                                                break;
                                                        }

                                                        case 16: //д����Ĵ���
                                                        {
                                                                Modbus_16_Solve();
                                                                break;
                                                        }


                                                }
                                                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        }
                                        else//CRCУ�����
                                        {

                                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                                RS485_TX_BUFF[2]=0x04; //�쳣��
                                                RS485_SendData(RS485_TX_BUFF,3);
                                        }
                                }
                                else//�Ĵ�����ַ������Χ
                                {
                                        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                        RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                        RS485_TX_BUFF[2]=0x02; //�쳣��
                                        RS485_SendData(RS485_TX_BUFF,3);
                                }
                        }
                        else//���������
                        {
                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                RS485_TX_BUFF[2]=0x01; //�쳣��
                                RS485_SendData(RS485_TX_BUFF,3);
                        }
          }

                RS485_FrameFlag=0;//��λ֡������־
                RS485_RX_CNT=0;//���ռ���������
                RS485_TX_EN=0;//��������ģʽ
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
	OLED_Refresh_Gram( );//������ʾ��OLED	
	while(1)
	{
		RS485_Service();
	}

}
*/