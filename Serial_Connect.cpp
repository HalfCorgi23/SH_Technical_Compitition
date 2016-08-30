#include "stdafx.h"
#include "Serial_Connect.h"

using namespace std;

/*声明串口*/
Serial_Connect::Serial_Connect()
{
}


Serial_Connect::~Serial_Connect()
{
}

/*串口初始化*/
int Serial_Connect::Serial_Init()
{
	Handle_Comm = CreateFile(
		_T("COM3:"),                  //COM3
		GENERIC_READ | GENERIC_WRITE, //读写权限
		0,                            //独占方式
		NULL,
		OPEN_EXISTING,                //直接打开
		0,                            //同步方式
		NULL
	);
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
		printf("写串口失败!!\n");
	}
	else
	{
		printf("写串口成功!!\n");
	}
	PurgeComm(Handle_Comm,
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

/*关闭串口*/
void Serial_Connect::Serial_Close()
{
	CloseHandle(Handle_Comm);
}

