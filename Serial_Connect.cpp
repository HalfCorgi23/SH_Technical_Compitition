#include "stdafx.h"
#include "Serial_Connect.h"

using namespace std;

/*��������*/
Serial_Connect::Serial_Connect()
{
}


Serial_Connect::~Serial_Connect()
{
}

/*���ڳ�ʼ��*/
int Serial_Connect::Serial_Init()
{
	Handle_Comm = CreateFile(
		_T("COM3:"),                  //COM3
		GENERIC_READ | GENERIC_WRITE, //��дȨ��
		0,                            //��ռ��ʽ
		NULL,
		OPEN_EXISTING,                //ֱ�Ӵ�
		0,                            //ͬ����ʽ
		NULL
	);
	if (Handle_Comm == (HANDLE)-1)
	{
		return 1;
	}
	else
	{
		//ͬ��I/O��ʽ�򿪴���
		SetupComm(Handle_Comm, 1024, 1024);  //�������������1024
		COMMTIMEOUTS TimeOuts;

		//�趨����ʱʱ��
		TimeOuts.ReadIntervalTimeout = 1000;
		TimeOuts.ReadTotalTimeoutMultiplier = 500;
		TimeOuts.ReadTotalTimeoutConstant = 5000;

		//�趨д��ʱʱ��
		TimeOuts.WriteTotalTimeoutMultiplier = 500;
		TimeOuts.WriteTotalTimeoutConstant = 2000;

		SetCommTimeouts(Handle_Comm, &TimeOuts);  //���ó�ʱ

		DCB Handle_Dcb;
		GetCommState(Handle_Comm, &Handle_Dcb);  //��ȡ����������Ϣ
												 //�޸Ĵ��ڲ���
		Handle_Dcb.BaudRate = 115200;  //������
		Handle_Dcb.ByteSize = 8;  //����λ8λ
		Handle_Dcb.Parity = NOPARITY;  //����ż����λ
		Handle_Dcb.StopBits = TWOSTOPBITS;  //����ֹͣλ

											//���洮����Ϣ
		SetCommState(Handle_Comm, &Handle_Dcb);
		//��ջ���
		PurgeComm(Handle_Comm, PURGE_TXCLEAR | PURGE_RXCLEAR);
		return 0;
	}
}

/*д����*/
void Serial_Connect::Serial_Write(char lpOut[1])
{
	DWORD dwBytesWrite = 1;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	ClearCommError(Handle_Comm, &dwErrorFlags, &ComStat);
	bWriteStat = WriteFile(Handle_Comm, lpOut, dwBytesWrite, &dwBytesWrite, NULL);
	if (!bWriteStat)
	{
		printf("д����ʧ��!!\n");
	}
	else
	{
		printf("д���ڳɹ�!!\n");
	}
	PurgeComm(Handle_Comm,
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

/*�رմ���*/
void Serial_Connect::Serial_Close()
{
	CloseHandle(Handle_Comm);
}

