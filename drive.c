#include <reg52.h>
#include "drive.h"
#include "fp_module.h"
#include "uart.h"



sbit led2 = P2^1;  //D2
sbit led3 = P2^2;  //D3
sbit led4 = P2^3;
/*void EnInt(char flag)
{
	if(flag == 1)
	{
		EA = 1;	  //�����ж�
	}
	else
	{
		EA =0;
	}
	return;
}

void EnInt_Ex0(char flag)
{
	if(flag == 1)
	{
		EX0 = 1;	  //���ⲿ�ж�0
	}
	else
	{
		EX0 =0;
	}
	return;
}

void EnInt_It0(char flag)
{//���ô�����ʽ
	if(flag == 1)
	{
		IT0 = 1;
	}
	else
	{
		IT0 = 0;
	}
}
 */
void Lock_Switch()
{
	led2 = 1;
	led3 = 0;
	System_Dly(50000);
	System_Dly(50000);
	System_Dly(50000);
	led2 = 0;
	led3 = 1;
	System_Dly(50000);
	System_Dly(50000);
	System_Dly(50000);
	led2 = 0;
	led3 = 0;
}

void Int0Init()
{
	//����INT0
	IT0=0;//�����س�����ʽ(������)
	EX0=1;//��INT0���ж�����	
	EA=1;//�����ж�	
}

void System_Dly(unsigned int count)//10000ԼΪ450ms
{
	while(count--);
}




void Timer0Init()
{
	TMOD|=0X01;//ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������

	TH0=0XFC;	//����ʱ������ֵ����ʱ1ms
	TL0=0X18;	
	ET0=1;//�򿪶�ʱ��0�ж�����
	EA=1;//�����ж�
	TR0=1;//�򿪶�ʱ��			
}

void Restart_Init()
{
	Timer0Init();
	Uart_Init_9600();
	//Uart_Init_115200();	
	Int0Init();
}


void Timer0() interrupt 1
{
	TH0=0XFC;	//����ʱ������ֵ����ʱ1ms
	TL0=0X18;
	if(FPCommMode.x1msDly>0)
	{
		FPCommMode.x1msDly--;
	}
	if(FPCommMode.x1msDly_FP_Process>0)
	{
		FPCommMode.x1msDly_FP_Process--;
	}	
}

