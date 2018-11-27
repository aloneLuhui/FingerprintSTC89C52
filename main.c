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


sbit led1 = P2^0;//led D1��
sbit led2 = P2^1;
sbit led3 = P2^2;

/*******************************************************************************
* �� �� ��       : main
* ��������		 : ������
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/

unsigned char flag = 0;
unsigned char sign = 0;

void main()
{
	Restart_Init();
	P0 = 0x7f;
	while(1)
	{
		if(FPCommMode.isWorkFlag == 1)
		{
			 led2 = 0;
			 FP_Process();
			 led2 = 1;
			 FPCommMode.isWorkFlag =0;
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


