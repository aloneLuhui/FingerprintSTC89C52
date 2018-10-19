#include <reg52.h>
#include "drive.h"
#include "fp_module.h"
#include "uart.h"

/*void EnInt(char flag)
{
	if(flag == 1)
	{
		EA = 1;	  //打开总中断
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
		EX0 = 1;	  //打开外部中断0
	}
	else
	{
		EX0 =0;
	}
	return;
}

void EnInt_It0(char flag)
{//设置触发方式
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


void Int0Init()
{
	//设置INT0
	IT0=0;//跳变沿出发方式(上升沿)
	EX0=1;//打开INT0的中断允许。	
	EA=1;//打开总中断	
}

void System_Dly(unsigned int count)//10000约为450ms
{
	while(count--);
}




void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。

	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器			
}

void Restart_Init()
{
	//Timer0Init();
	Uart_Init_9600();
	//Uart_Init_115200();	
	//Int0Init();
}


void Timer0() interrupt 1
{
	static unsigned int i;
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
	i++;
	if(i==1000)
	{
		i=0;
		FPCommMode.x1msDly--;
	}	
}

