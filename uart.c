#include <reg52.h>
//#include <stdio.h>
#include "uart.h"
#include "fp_module.h"
#include "drive.h"

sbit led6 = P2^5;
sbit led7 = P2^6;
sbit led8 = P2^7;

char idata FPUartRxdBuf[FP_UART_RXD_BUF_SIZE];
char idata FPRxdData[FP_UART_RXD_DEAL_SIZE];
UartRxdQueue_t FPUartRxdQue;
static int rxlen = 0;
//unsigned char idata rx[20];
int rxlenth = 0;
	
void FP_UartDataInit()
{

	
	FPUartRxdQue.Qin = 0;
	FPUartRxdQue.Qout = 0;
	FPUartRxdQue.Qbuf = FPUartRxdBuf;

	rxlen = 0;
}


void FP_UartRxdTask()
{
	int tmp =0;
	int i = 0;
	static char idata buf[30];
	if(FPUartRxdQue.Qout == FPUartRxdQue.Qin)
	{
		return;
	}

	buf[rxlen++] = FPUartRxdQue.Qbuf[FPUartRxdQue.Qout++];
	if(FPUartRxdQue.Qout >= FP_UART_RXD_BUF_SIZE)
	{
		FPUartRxdQue.Qout = 0;
	}       
	switch(rxlen)
	{
		case 1:  //�ӽ��յ�������ȡ������ͷ��һ�ֽ�
			if(FP_Pack_Head[0] != buf[0])
			{
				rxlen = 0;
			}
			break;
		case 2:  //�ӽ��յ�������ȡ������ͷ�ڶ��ֽ�
			if(FP_Pack_Head[1] != buf[1])
			{
				rxlen = 0;
			}
			break;
		case 3:
		case 4:
		case 5:
		case 7:
		case 8:
		case 9:
			break;
		case 6:
			if((FP_Pack_Head[2] != buf[2]) || (FP_Pack_Head[3] != buf[3]) 
			|| (FP_Pack_Head[4] != buf[4]) || (FP_Pack_Head[5] != buf[5]))
			{
				rxlen = 0;
			}
			break;
		default:
			tmp = ((int)buf[7] << 8) + (int)buf[8];
			while(rxlen < tmp+9)
			{
				if(FPUartRxdQue.Qout == FPUartRxdQue.Qin)
				{
					break;  //û���������˳�
				}
				if(rxlen >= FP_UART_RXD_DEAL_SIZE)
				{
					rxlen = 0;
					break;
				}
				buf[rxlen++] = FPUartRxdQue.Qbuf[FPUartRxdQue.Qout++];
				if(FPUartRxdQue.Qout >= FP_UART_RXD_BUF_SIZE)
				{
					FPUartRxdQue.Qout = 0;
				}
			}
			break;
	}

	tmp = ((int)buf[7] << 8) + (int)buf[8]; //���յ���ȷ���ĳ���
	if((rxlen > 9) && (rxlen >= tmp+9))
	{
		tmp = 0;
		for(i=6; i<rxlen-2; i++)
		{
			tmp += buf[i]; //У���
		}
		if(((char)(tmp >> 8) == buf[rxlen-2]) && ((char)tmp == buf[rxlen-1])) //�ж�У����Ƿ���ȷ
		{
			//ֻ�д��ڵȴ�ָ��ͷӦ��ʱ�Ŵ�����յ�������
			if(FPMODE_CMD_RECEIVING == FPCommMode.Index)
			{
			if(0x07 == buf[6])  //Ӧ���
			{
				switch(FPCommMode.IndexBak)
				{
				case FPMODE_CMD_GET_IMAGE1:
				case FPMODE_CMD_GEN_CHAR1:
				case FPMODE_CMD_SEARCH_ALL:
				case FPMODE_CMD_GET_IMAGE2:
				case FPMODE_CMD_GEN_CHAR2:
				case FPMODE_CMD_REG_MODEL:
				case FPMODE_CMD_STORE_MODEL:
					FPCommMode.rxlen = rxlen;
					for(i=0; i<rxlen; i++)
					{
						FPCommMode.rxdata[i] = buf[i];
					}
					if(0 == buf[9])
					{
						FPCommMode.result = RESULT_OK;
						led8 = 0;
						System_Dly(5000);
						led8 = 1;
					}
					else
					{
						FPCommMode.result = RESULT_FAIL;
						led7 = 0;
						System_Dly(5000);
						led7 = 1;
					}
					break;
					
				default:
					break;
				}
			}
			}
		}
		rxlen = 0;
	}

}

void Uart_Init_9600()  
{
    SCON = 0x50 ; //���ڹ�����ʽ1��8λUART�������ʿɱ�
    TMOD |= 0x20 ; //��ʱ��1��������ʽ2���Զ���װ��8λ��ʱ�� 
    PCON |= 0x80 ; //SMOD=1; �����ʼӱ�
    TH1 = 0xfa ; //������:9600 ����=11.0592MHz 
    IE |= 0x90 ; //ʹ�ܴ����ж� 
    TR1 = 1 ; // ��ʱ��1��ʼ 
}




void Uart_Init_115200() 
{ 
     SCON=0x50; //���ڹ�����ʽ1��8λUART�������ʿɱ�  
     TH2=0xFF;           
     TL2=0xFD;    //������:115200 ����=11.0592MHz 
     RCAP2H=0xFF;   
     RCAP2L=0xFD; //16λ�Զ���װ��ֵ
/*****************/
     TCLK=1;   
     RCLK=1;   
     C_T2=0;   
     EXEN2=0; //�����ʷ�����������ʽ
/*****************/
	 TR2=1 ; //��ʱ��2��ʼ
}



void Uart_Send(char* _data,int _len)
{
	int i = 0;
	for(i=0;i<_len;i++)
	{
		SBUF = _data[i];
		while(TI == 0);
		TI = 0;
	}
}


/*void Uart_Test()
{
	int i=0;
	
	//unsigned char idata rx[20];
	if(FPUartRxdQue.Qout == FPUartRxdQue.Qin)
	{
		//Uart_Send("a",1);
		return;
	}
	while(FPUartRxdQue.Qout < FPUartRxdQue.Qin)
	{
		rx[i++] = FPUartRxdQue.Qbuf[FPUartRxdQue.Qout++];
		//Uart_Send("b",1);
	}
	if(rx[9]==0x00)
	{
		FPCommMode.result = RESULT_OK;
	}

}
  */







void Serial_Int() interrupt	4
{
	int Qin;
	if(RI)
	{
		RI=0;	//������жϱ�־
		FPUartRxdQue.Qbuf[FPUartRxdQue.Qin] = SBUF;
		Qin = FPUartRxdQue.Qin + 1;
		if(Qin >= FP_UART_RXD_BUF_SIZE)
		{
			Qin = 0;
		}
		if(Qin != FPUartRxdQue.Qout)
		{
			FPUartRxdQue.Qin = Qin;
		}	
	} 
	/*if(RI)
	{
		RI=0;
		rx[rxlenth++]=SBUF;
		if(rxlenth==20)
		{
			rxlenth = 0;
		}
	}  */
	 /*
	if(0)
	{
		SBUF = _temp;
		while(TI ==0);
		TI =0;
	}  */
	
}	   
