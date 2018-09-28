#include <reg52.h>
//#include <stdio.h>
#include "uart.h"
#include "fp_module.h"
#include "drive.h"

char idata FPUartRxdBuf[FP_UART_RXD_BUF_SIZE];
//char FPRxdData[FP_UART_RXD_DEAL_SIZE];
UartRxdQueue_t FPUartRxdQue;
static int rxlen = 0;
	
void FP_UartDataInit()
{

	
	FPUartRxdQue.Qin = 0;
	FPUartRxdQue.Qout = 0;
	FPUartRxdQue.Qbuf = FPUartRxdBuf;

	rxlen = 0;
}


void FP_UartRxdTask()
{
	int datalen =0;
	int tmp =0;
	int i = 0;
	static char idata buf[50];
	
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
			datalen = tmp;
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
					FPCommMode.rxlen = rxlen;
					for(i=0; i<rxlen; i++)
					{
						FPCommMode.rxdata[i] = buf[i];
					}
					if(0 == buf[9])
					{
						FPCommMode.result = RESULT_OK;
					}
					else
					{
						FPCommMode.result = RESULT_FAIL;
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
     TMOD=0x20;    //��ʱ��������ʽ��ѡ���˶�ʱ��1��������ʽ2 ��λ��ֵ�Զ���װ��8λ��ʱ����          
     TH1=0xfd;     //��ʱ��1��ֵ  ,���ò�����Ϊ9600 ����11.0529MHZ?  
     TL1=0xfd;  
     TR1=1;        //������ʱ��1  
   
     SM0=0;  
     SM1=1;        //10λ�첽���գ���8λ���ݣ������ʿɱ�  
     REN=1;        //�����пڽ���λ  
     EA=1;         //�����жϣ���բ��  
     ES=1;         //�������ж�  
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


void Uart_Test()
{
	#if(1)
	{
		Uart_Send("Hello", 5);
		System_Dly(50000);
	}
}








void Serial_Int() interrupt	4
{
	unsigned char _temp;
	int Qin;
	if(RI)
	{
		RI=0;	//������жϱ�־
		_temp = SBUF;	
	}

	#if(1)
	{
		SBUF = _temp;
		while(TI ==0);
		TI =0;
	}
	#endif
	
	FPUartRxdQue.Qbuf[FPUartRxdQue.Qin] = _temp;
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
