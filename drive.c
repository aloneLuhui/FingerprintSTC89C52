#include <reg52.h>
#include "drive.h"
#include "fp_module.h"
#include "uart.h"

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
}


void Timer0() interrupt 1
{
	static unsigned int i;
	TH0=0XFC;	//����ʱ������ֵ����ʱ1ms
	TL0=0X18;
	i++;
	if(i==1000)
	{
		i=0;
		FPCommMode.x1msDly--;
	}	
}