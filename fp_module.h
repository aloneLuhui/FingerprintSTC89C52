#ifndef __FP_MODULE_H_
#define __FP_MODULE_H_



typedef enum
{
  FPMODE_HANDLE_INIT = 0x00,
  FPMODE_CMD_GET_IMAGE1,   //��ȡָ��ͼ��1
  FPMODE_CMD_GEN_CHAR1,    //��������1
  FPMODE_CMD_SEARCH_ALL,   //����ָ�ƿ�
  FPMODE_CMD_GET_IMAGE2,
  FPMODE_CMD_GEN_CHAR2,
  FPMODE_CMD_REG_MODEL,
  FPMODE_CMD_STORE_MODEL,
  FPMODE_CMD_RECEIVING,    //
  FPMODE_OPEN_DOOR,
  FPMODE_SEARCH_FAIL,
  FPMODE_HANDLE_NULL = 0xff,
} FPmode_index_et;



typedef struct
{
  char  result;  //
  char  error;   //
  //char  FP_id;   //ָ�������ɹ��õ���ID
  //char  FP_userid;
  //int txlen;   //���ͳ���
  int rxlen;   //�������ݳ���
  int x1msDly;
  int BellOpenx1msDly;
  unsigned int x1msDly_FP_Process;
  FPmode_index_et Index;
  FPmode_index_et IndexBak;  //��ָ��ͷ������ȴ�����ʱ���õ����
  char  *rxdata;  //����ָ����Ϣ����
  //int rxnum;//��������
  //char isFinishFlag;//������ɱ�־
  char isWorkFlag;
  char isSampleFlag; 
  char StoreNum;
} FPCommMode_t;

extern FPCommMode_t FPCommMode;

#define RESULT_FAIL      0
#define RESULT_OK        1
#define RESULT_WAITING   2
#define TRUE 0
#define FALSE 1

extern char idata FP_Pack_Head[6];
extern char idata FP_Get_Img[6];
extern char idata FP_Search[11];
extern char idata FP_Img_To_Buffer1[7];
extern char idata FP_Img_To_Buffer2[7];
extern char idata FP_Reg_Model[6];
extern char idata FP_Save_Finger[9];

void FP_ModeReceiveOk(void);
void FP_ModeReceiveFail(char timeout);
void FP_CommModeTask(void);
void FP_SendConstCmd(char *txbuf, int len);
void Match_init();
void FP_Process();
void FP_SearchAllResultProc(void);
void FP_CommModeInit(void);
void FP_SendSaveFinger(void);
#endif