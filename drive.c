#include <reg52.h>
#include "drive.h"
#include "fp_module.h"
#include "uart.h"
#include "i2c.h"


sbit led2 = P2^1;  //D2
sbit led3 = P2^2;  //D3
sbit led4 = P2^3;
sbit led5 = P2^4;
sbit led6 = P2^5;
sbit beep = P1^5;	
//sbit pin0 = P0^0;
//sbit pin1 = P1^1; 
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
	led5 = 1;
	led6 = 0;
	//P0^0 = 1;
	//P0^1 = 0;
	//pin0 = 1;
	//pin1 = 0;
	//System_Dly(50000);
	System_Dly(50000);
	System_Dly(50000);
	led5 = 0;
	led6 = 0;
	//P0^0=0;
	//P0^1=0;
	//pin0 = 0;
	//pin1 = 0;
	System_Dly(50000);
	System_Dly(50000);
	System_Dly(50000);
	System_Dly(50000);
	//System_Dly(50000);
	//System_Dly(50000);
	led5 = 0;
	led6 = 1;
	//P0^0=0;
	//P0^1=1;
	//pin0 = 0;
	//pin1 = 1;
	//System_Dly(50000);
	System_Dly(50000);
	System_Dly(50000);
	led5 = 0;
	led6 = 0;
	//P0^0=0;
	//P0^1=0;
	//pin0 = 0;
	//pin1 = 0;
}

void Int0Init()
{
	//����INT0
	IT0=1;//�����س�����ʽ(�½���)
	EX0=1;//��INT0���ж�����	
	EA=1;//�����ж�	
}

void Int1Init()
{
	IT1=1;
	EX1=1;
	EA=1;
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
	//P0^0=0;
	//P0^1=0;
	//pin0 = 0;
	//pin1 = 0;
	P0 = 0x00;
	Timer0Init();
	Uart_Init_9600();
	//Uart_Init_115200();	
	Int0Init();
	Int1Init();
	FPCommMode.isWorkFlag = 0;
	FPCommMode.isSampleFlag = 0;
	FPCommMode.StoreNum = At24c02Read(1);
	if(FPCommMode.StoreNum >= 0x64)//100
	{
		 FPCommMode.StoreNum = 0;
		 At24c02Write(1,FPCommMode.StoreNum);
	}
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
	if(FPCommMode.BellOpenx1msDly>0)
	{
		FPCommMode.BellOpenx1msDly--;
	}	
}

void Bell_Open(int x1ms)
{
	FPCommMode.BellOpenx1msDly =  x1ms;
	while(FPCommMode.BellOpenx1msDly>0)
	{
		beep = ~beep;
		System_Dly(10);
	}
	beep = 1;

}





