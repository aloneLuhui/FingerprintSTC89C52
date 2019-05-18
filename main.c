/**************************************************************************************
*		              ���ļ�����51��Ƭ����ָ����												  *
ʵ���������س����D1ָʾ�Ƶ���
ע�������																				  
***************************************************************************************/


#include "reg52.h"    //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include "common.h"
#include "main.h"
#include "fp_module.h"
#include "uart.h"
#include "drive.h"
#include "i2c.h"
#include "lcd.h"
sbit led1 = P2^0;//led D1��
sbit led2 = P2^1;
sbit led3 = P2^2;
sbit led4 = P2^3;
sbit led5 = P2^4;
sbit led6 = P2^5;
//char xdata ma[200];
/*******************************************************************************
* �� �� ��       : main
* ��������		 : ������
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void main()
{
	Restart_Init();
	while(1)
	{
		led5 = 0;
		led6 = 0;
		if(FPCommMode.isWorkFlag == 1)
		{
			//EX0=0;//�ر�INT0���ж�����
			 led2 = 0;
			 FP_Process();
			 led2 = 1;
			 FPCommMode.isWorkFlag =0;
			 FPCommMode.isSampleFlag = 0;
			 //EX0=1;//��INT0���ж�����
		}
	}
}




void Int0()	interrupt 0		//�ⲿ�ж�0���жϺ���
{
	System_Dly(1000);	 //��ʱ����
	led1 = ~led1;
	if(P3^2==1)
	{
		FPCommMode.isWorkFlag = 1;
	}
}

void Int1() interrupt 2
{
	System_Dly(1000);
	if(P3^3==0)
	{
		FPCommMode.isSampleFlag = 1;
		FPCommMode.isWorkFlag = 1;
	}
}

