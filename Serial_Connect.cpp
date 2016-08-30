#include "stdafx.h"
#include "Serial_Connect.h"

#define AIMED_Y_MIN 210

using namespace std;

/*��������*/
Serial_Connect::Serial_Connect(int serial_num)
{
	if (serial_num == 1)
	{
		Handle_Comm = CreateFile(_T("COM1:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM1����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
	else if (serial_num == 2)
	{
		Handle_Comm = CreateFile(_T("COM2:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM2����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
	else if (serial_num == 3)
	{
		Handle_Comm = CreateFile(_T("COM3:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM3����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
	else if (serial_num == 4)
	{
		Handle_Comm = CreateFile(_T("COM4:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM4����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
	else if (serial_num == 5)
	{
		Handle_Comm = CreateFile(_T("COM5:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM5����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
	else if (serial_num == 6)
	{
		Handle_Comm = CreateFile(_T("COM6:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM6����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
	else if (serial_num == 7)
	{
		Handle_Comm = CreateFile(_T("COM7:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM7����дȨ�ޣ���ռ��ʽ��ֱ�Ӵ򿪣�ͬ����ʽ
	}
}

Serial_Connect::~Serial_Connect()
{
}

/*���ڳ�ʼ��*/
int Serial_Connect::Serial_Init()
{
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
		printf("д����ʧ�ܣ�״̬��");
	}
	else
	{
		printf("д���ڳɹ���״̬��");
	}
	PurgeComm(Handle_Comm,
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

/*������*/
char Serial_Connect::Serial_Read()
{
	char str[1];
	DWORD wCount;
	BOOL bReadStat;
	bReadStat = ReadFile(Handle_Comm, str, 1, &wCount, NULL);
	if (!bReadStat)
	{
		printf_s("������ʧ�ܣ�");
	}
	else
	{
		printf_s("�����ڳɹ���");
	}
	return str[0];
}

/*�رմ���*/
void Serial_Connect::Serial_Close()
{
	CloseHandle(Handle_Comm);
}

//����ָ��
void Serial_Connect::Send_Command(int x,int y)
{
	int aimed_y_min = AIMED_Y_MIN;
	int aimed_y_max = AIMED_Y_MIN + 70;
	int aim_y_min = AIMED_Y_MIN - 35;
	int aim_y_max = aimed_y_max + 35;
	int aimed_x_min = 280;
	int aimed_x_max = 360;
	int aim_x_min = 220;
	int aim_x_max = 420;
	if (x == -1 && y == -1)
	{
		Serial_Write("4");  //ֻת
		printf_s("δ����Ŀ��\n");
	}
	else
	{
		if (x <= aim_x_min)
		{
			Serial_Write("5"); //��ת
			printf_s("Ŀ������Ұ���\n");
		}
		else if (x >= aim_x_max)
		{
			Serial_Write("4"); //��ת
			printf_s("Ŀ������Ұ�Ҳ�\n");
		}
		else
		{
			if (y <= aim_y_min)
			{
				Serial_Write("b"); //��̨���ϣ�����ǰ
				printf_s("Ŀ������Ұ�ϲ�\n");
			}
			else if (y >= aim_y_max)
			{
				Serial_Write("c"); //��̨���£������
				printf_s("Ŀ������Ұ�²�\n");
			}
			else
			{
				if (x <= aimed_x_min)
				{
					Serial_Write("7"); //��ת����
					printf_s("Ŀ������Ұ����ƫ��\n");
				}
				else if (x >= aimed_x_max)
				{
					Serial_Write("8"); //��ת����
					printf_s("Ŀ������Ұ����ƫ��\n");
				}
				else
				{
					if (y <= aimed_y_min)
					{
						Serial_Write("a"); //��̨�����ᣬ��
						printf_s("Ŀ������Ұ����ƫ��\n");
					}
					else if (y >= aimed_y_max)
					{
						Serial_Write("9"); //��̨����ѹ����
						printf_s("Ŀ������Ұ����ƫ��\n");
					}
					else
					{
						Serial_Write("6");
						printf_s("Ŀ������Ұ����\n");
					}
				}
			}
		}
	}
}

