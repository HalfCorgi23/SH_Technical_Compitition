#include "stdafx.h"
#include "Serial_Connect.h"

#define AIMED_Y_MIN 210

using namespace std;

/*声明串口*/
Serial_Connect::Serial_Connect(int serial_num)
{
	if (serial_num == 1)
	{
		Handle_Comm = CreateFile(_T("COM1:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM1，读写权限，独占方式，直接打开，同步方式
	}
	else if (serial_num == 2)
	{
		Handle_Comm = CreateFile(_T("COM2:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM2，读写权限，独占方式，直接打开，同步方式
	}
	else if (serial_num == 3)
	{
		Handle_Comm = CreateFile(_T("COM3:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM3，读写权限，独占方式，直接打开，同步方式
	}
	else if (serial_num == 4)
	{
		Handle_Comm = CreateFile(_T("COM4:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM4，读写权限，独占方式，直接打开，同步方式
	}
	else if (serial_num == 5)
	{
		Handle_Comm = CreateFile(_T("COM5:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM5，读写权限，独占方式，直接打开，同步方式
	}
	else if (serial_num == 6)
	{
		Handle_Comm = CreateFile(_T("COM6:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM6，读写权限，独占方式，直接打开，同步方式
	}
	else if (serial_num == 7)
	{
		Handle_Comm = CreateFile(_T("COM7:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//COM7，读写权限，独占方式，直接打开，同步方式
	}
}

Serial_Connect::~Serial_Connect()
{
}

/*串口初始化*/
int Serial_Connect::Serial_Init()
{
	if (Handle_Comm == (HANDLE)-1)
	{
		return 1;
	}
	else
	{
		//同步I/O方式打开串口
		SetupComm(Handle_Comm, 1024, 1024);  //输入输出缓冲区1024
		COMMTIMEOUTS TimeOuts;

		//设定读超时时间
		TimeOuts.ReadIntervalTimeout = 1000;
		TimeOuts.ReadTotalTimeoutMultiplier = 500;
		TimeOuts.ReadTotalTimeoutConstant = 5000;

		//设定写超时时间
		TimeOuts.WriteTotalTimeoutMultiplier = 500;
		TimeOuts.WriteTotalTimeoutConstant = 2000;

		SetCommTimeouts(Handle_Comm, &TimeOuts);  //设置超时

		DCB Handle_Dcb;
		GetCommState(Handle_Comm, &Handle_Dcb);  //获取串口配置信息
												 //修改串口参数
		Handle_Dcb.BaudRate = 115200;  //波特率
		Handle_Dcb.ByteSize = 8;  //数据位8位
		Handle_Dcb.Parity = NOPARITY;  //无奇偶检验位
		Handle_Dcb.StopBits = TWOSTOPBITS;  //两个停止位

											//保存串口信息
		SetCommState(Handle_Comm, &Handle_Dcb);
		//清空缓存
		PurgeComm(Handle_Comm, PURGE_TXCLEAR | PURGE_RXCLEAR);
		return 0;
	}
}

/*写串口*/
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
		printf("写串口失败，状态：");
	}
	else
	{
		printf("写串口成功，状态：");
	}
	PurgeComm(Handle_Comm,
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

/*读串口*/
char Serial_Connect::Serial_Read()
{
	char str[1];
	DWORD wCount;
	BOOL bReadStat;
	bReadStat = ReadFile(Handle_Comm, str, 1, &wCount, NULL);
	if (!bReadStat)
	{
		printf_s("读串口失败！");
	}
	else
	{
		printf_s("读串口成功！");
	}
	return str[0];
}

/*关闭串口*/
void Serial_Connect::Serial_Close()
{
	CloseHandle(Handle_Comm);
}

//发送指令
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
		Serial_Write("4");  //只转
		printf_s("未发现目标\n");
	}
	else
	{
		if (x <= aim_x_min)
		{
			Serial_Write("5"); //左转
			printf_s("目标在视野左侧\n");
		}
		else if (x >= aim_x_max)
		{
			Serial_Write("4"); //右转
			printf_s("目标在视野右侧\n");
		}
		else
		{
			if (y <= aim_y_min)
			{
				Serial_Write("b"); //云台向上，车向前
				printf_s("目标在视野上侧\n");
			}
			else if (y >= aim_y_max)
			{
				Serial_Write("c"); //云台向下，车向后
				printf_s("目标在视野下侧\n");
			}
			else
			{
				if (x <= aimed_x_min)
				{
					Serial_Write("7"); //左转，打
					printf_s("目标在视野中央偏左\n");
				}
				else if (x >= aimed_x_max)
				{
					Serial_Write("8"); //右转，打
					printf_s("目标在视野中央偏右\n");
				}
				else
				{
					if (y <= aimed_y_min)
					{
						Serial_Write("a"); //云台向上提，打
						printf_s("目标在视野中央偏上\n");
					}
					else if (y >= aimed_y_max)
					{
						Serial_Write("9"); //云台向下压，打
						printf_s("目标在视野中央偏下\n");
					}
					else
					{
						Serial_Write("6");
						printf_s("目标在视野中央\n");
					}
				}
			}
		}
	}
}

