#include"common.h"



void Utility_Strncpy ( char* dest,char* src, int Len )
{
	int i = 0;
	//��������ӽ�����
	for (i=0; i<Len; i++ )
	{
		dest[i] = src[i];
	}
}