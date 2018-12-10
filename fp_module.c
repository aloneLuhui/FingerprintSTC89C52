#include"fp_module.h"
#include "common.h"
#include "uart.h"
#include "drive.h"
#include "i2c.h"

#define FP_COMM_GET_IMAGE1_DELAY       4000  //��һ�λ�ȡͼ��Ҫ�Ⱦ�һ��
#define FP_COMM_SEND_GET_IMAGE1_DELAY  6   //��һ�λ�ȡͼ������ʱ����Ϊ��ѭ������
#define FP_COMM_RXD_X1MS_DELAY        2000

char idata FP_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //��ͷ
char idata FP_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //��ȡͼ��1
char idata FP_Search[11]= {0x01,0x0,0x08,0x04,0x01,0x00,0x00,0x00,0xC8,0x00,0xD6}; //����
char idata FP_Img_To_Buffer1[7]={0x01,0x0,0x04,0x02,0x01,0x0,0x08}; //����char1
char idata FP_Img_To_Buffer2[7]={0x01,0x0,0x04,0x02,0x02,0x0,0x09}; //����char2
char idata FP_Reg_Model[6]={0x01,0x0,0x03,0x05,0x0,0x09}; //�ϳ�ģ��
char idata FP_Save_Finger[9]= {0x01,0x00,0x06,0x06,0x01,0x00,0x00,0x00,0x0e};//����char1��ָ��

FPCommMode_t FPCommMode;


void FP_ModeReceiveOk(void)
{
	switch(FPCommMode.IndexBak)
	{
	case FPMODE_CMD_GET_IMAGE1:
		FPCommMode.Index = FPMODE_CMD_GEN_CHAR1;
		break;
		
	case FPMODE_CMD_GEN_CHAR1:
		if(FPCommMode.isSampleFlag == 1)
		{
			FPCommMode.Index = FPMODE_CMD_GET_IMAGE2;
			System_Dly(10000);
		}
		else
		{
			FPCommMode.Index = FPMODE_CMD_SEARCH_ALL;
		}
		break;

	case FPMODE_CMD_SEARCH_ALL:
		FP_SearchAllResultProc();
		break;

	case FPMODE_CMD_GET_IMAGE2:
		FPCommMode.Index = FPMODE_CMD_GEN_CHAR2;
		break;
		
	case FPMODE_CMD_GEN_CHAR2:
		FPCommMode.Index = FPMODE_CMD_REG_MODEL;
		break;
		
	case FPMODE_CMD_REG_MODEL:
		FPCommMode.Index = FPMODE_CMD_STORE_MODEL;
		break;

	case FPMODE_CMD_STORE_MODEL:
		FPCommMode.Index = FPMODE_HANDLE_NULL;
		FPCommMode.isSampleFlag = 0;
		//FPCommMode.x1msDly = 0;
		FPCommMode.x1msDly_FP_Process = 0;
		Bell_Open(200);
		if(FPCommMode.StoreNum>=0x64)
		{
			FPCommMode.StoreNum = 0;
		}
		else
		{
			FPCommMode.StoreNum += 1;
		}
		At24c02Write(1,FPCommMode.StoreNum);
		//System_Dly(10000);
		//Bell_open(200);
		break;
		

	default:
		FPCommMode.Index = FPMODE_HANDLE_INIT;
		break;
	}
}





void FP_ModeReceiveFail(char timeout)
{
	switch(FPCommMode.IndexBak)
	{
	case FPMODE_CMD_GET_IMAGE1:
		FPCommMode.Index = FPMODE_HANDLE_INIT;
		break;
		
	case FPMODE_CMD_GEN_CHAR1:
		FPCommMode.Index = FPMODE_HANDLE_INIT;
		break;
		
	case FPMODE_CMD_SEARCH_ALL:
		FP_SearchAllResultProc();
		break;

	case FPMODE_CMD_GET_IMAGE2:
		FPCommMode.Index = FPMODE_CMD_GET_IMAGE2;
		break;
		
	case FPMODE_CMD_GEN_CHAR2:
		FPCommMode.Index = FPMODE_CMD_GET_IMAGE2;	
		break;
		
	case FPMODE_CMD_REG_MODEL:
		//�ϳ�ģ��ʧ�ܣ�ֹͣ�ɼ�
		FPCommMode.Index = FPMODE_HANDLE_NULL;
		FPCommMode.isSampleFlag = 0;
		//FPCommMode.x1msDly = 0;
		FPCommMode.x1msDly_FP_Process = 0;
		break;

	case FPMODE_CMD_STORE_MODEL:
		FPCommMode.Index = FPMODE_HANDLE_NULL;
		FPCommMode.isSampleFlag = 0;
		//FPCommMode.x1msDly = 0;
		FPCommMode.x1msDly_FP_Process = 0;
		Bell_Open(200);
		System_Dly(10000);
		Bell_Open(200);
		break;
		
	default:
		FPCommMode.Index = FPMODE_HANDLE_INIT;
		break;
	}

	if(TRUE == timeout)
	{
		FPCommMode.Index = FPMODE_HANDLE_INIT;
	}
}


void FP_CommModeTask(void)
{
	switch(FPCommMode.Index)
	{
	case FPMODE_HANDLE_INIT:
		FPCommMode.Index = FPMODE_CMD_GET_IMAGE1;
		break;
		
	case FPMODE_CMD_GET_IMAGE1:
		FPCommMode.result = RESULT_WAITING;
		FPCommMode.x1msDly = FP_COMM_GET_IMAGE1_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_GET_IMAGE1;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		FP_SendConstCmd(FP_Get_Img, 6);
		break;
	
	case FPMODE_CMD_GEN_CHAR1:
		FPCommMode.result = RESULT_WAITING;
		FPCommMode.x1msDly = FP_COMM_RXD_X1MS_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_GEN_CHAR1;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		FP_SendConstCmd(FP_Img_To_Buffer1, 7);
		break;

	case FPMODE_CMD_GET_IMAGE2:
		FPCommMode.result = RESULT_WAITING;
		FPCommMode.x1msDly = FP_COMM_GET_IMAGE1_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_GET_IMAGE2;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		FP_SendConstCmd(FP_Get_Img, 6);
		break;

	case FPMODE_CMD_GEN_CHAR2:
		FPCommMode.result = RESULT_WAITING;
		FPCommMode.x1msDly = FP_COMM_RXD_X1MS_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_GEN_CHAR2;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		FP_SendConstCmd(FP_Img_To_Buffer2, 7);
		break;

	case FPMODE_CMD_REG_MODEL:
		FPCommMode.result = RESULT_WAITING;
		FPCommMode.x1msDly = FP_COMM_RXD_X1MS_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_REG_MODEL;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		FP_SendConstCmd(FP_Reg_Model, 6);
		break;

	case FPMODE_CMD_STORE_MODEL:
	 	FP_SendSaveFinger();
		FPCommMode.result = RESULT_WAITING;
		FPCommMode.x1msDly = FP_COMM_RXD_X1MS_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_STORE_MODEL;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		break;

	case FPMODE_CMD_SEARCH_ALL:
		FPCommMode.result = RESULT_WAITING;
		//FPCommMode.FP_id = FP_COMM_INVAID_ID;
		FPCommMode.x1msDly = FP_COMM_RXD_X1MS_DELAY;
		FPCommMode.IndexBak = FPMODE_CMD_SEARCH_ALL;
		FPCommMode.Index = FPMODE_CMD_RECEIVING;
		FP_SendConstCmd(FP_Search, 11);
		break;
		
	case FPMODE_CMD_RECEIVING:
		if(RESULT_OK == FPCommMode.result)
		{
			FP_ModeReceiveOk();
		}
		else if(RESULT_FAIL == FPCommMode.result)
		{
			FP_ModeReceiveFail(FALSE);
		}
		else if(0 == FPCommMode.x1msDly) //���ճ�ʱ
		{
			FP_ModeReceiveFail(TRUE);
		}
		break;

	case FPMODE_OPEN_DOOR:
		FPCommMode.Index = FPMODE_HANDLE_NULL;
		Bell_Open(100);
		Lock_Switch();
		FPCommMode.x1msDly_FP_Process = 0;
		break;

	case FPMODE_SEARCH_FAIL:
		//if(0 == FPCommMode.x1msDly)
		//{
		FPCommMode.Index = FPMODE_HANDLE_NULL;
		FPCommMode.x1msDly_FP_Process = 0;
		//}
		Bell_Open(200);
		System_Dly(10000);
		Bell_Open(200);
		break;
		
	case FPMODE_HANDLE_NULL:
		break;
		
	default:
		FPCommMode.Index = FPMODE_HANDLE_INIT;
		break;
	}
}




void FP_SendConstCmd(char *txbuf, int len)
{
       char sendcmd[20] = {0};

	Utility_Strncpy(sendcmd,FP_Pack_Head, 6);
	Utility_Strncpy(sendcmd + 6 ,txbuf, len);

	Uart_Send(sendcmd,len+6);	

}


//��������������
void FP_SearchAllResultProc(void)
{
	int pageID, score;
	pageID = ((int)FPCommMode.rxdata[10] << 8) + FPCommMode.rxdata[11];
	score  = ((int)FPCommMode.rxdata[12] << 8) + FPCommMode.rxdata[13];
	//���ؽ����Ϊ0���ҵ÷ִ���60��ӡ����
	//FPCommMode.FP_id = (char)pageID;
	if((score >= 60) && (0 == FPCommMode.rxdata[7]) && (7 == FPCommMode.rxdata[8]))
	{
		FPCommMode.Index = FPMODE_OPEN_DOOR;
		FPCommMode.x1msDly = 0;
	}
	else
	{
		FPCommMode.Index = FPMODE_SEARCH_FAIL;
		FPCommMode.x1msDly = 0;
		//FPCommMode.x1sDly = 100;
		//FPCommMode.xBellFailed10msDly =100;
		//FPCommMode.x1msDly_FP_Process = 0;
	}
}

//����ָ��
void FP_SendSaveFinger(void)
{
	char i;
	int temp = 0;
	FP_Save_Finger[6] = FPCommMode.StoreNum+1;
	for(i=0; i<7; i++)   //����У���
	{
		temp += FP_Save_Finger[i];
	}
	FP_Save_Finger[7]= (char)(temp >> 8); //���У������
	FP_Save_Finger[8]= (char)temp;
	FP_SendConstCmd(FP_Save_Finger, 9);
}



void Match_init()
{
	FPCommMode.Index = FPMODE_HANDLE_INIT;
}

void FP_CommModeInit(void)
{
	FPCommMode.result = RESULT_FAIL;
	FPCommMode.x1msDly = 0;
	FPCommMode.IndexBak = FPMODE_HANDLE_NULL;
	FPCommMode.rxdata = FPRxdData;

}

void FP_Process()
{
	 FPCommMode.x1msDly_FP_Process = 10000;  //10s
	 FP_UartDataInit();
	 FP_CommModeInit();
	 Match_init();
	 
	while(FPCommMode.x1msDly_FP_Process > 0)
	{
		FP_CommModeTask();
		System_Dly(10);
		FP_UartRxdTask();
	} 
	/*
	while(repeat > 0)
	{
		repeat--;
		rxlenth =0;
		FP_CommModeTask();
		System_Dly(1000);
		Uart_Test();
	}  */
}
