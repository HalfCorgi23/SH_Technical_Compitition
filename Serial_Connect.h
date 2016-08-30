#include <Windows.h>
#include <iostream>


using namespace std;

#pragma once
/*串口连接
声明串口：Serial_Connect(string)
串口0~9：“COM1:”
串口10及以上：“.\\COM10:”
(默认COM3)
初始化串口：Serial_Init()
写一位串口：Serial_Write(char)
关闭串口：Serial_Close()
*/
class Serial_Connect
{
public:
	Serial_Connect();
	~Serial_Connect();

	//初始化串口
	int Serial_Init();

	//写一位串口
	void Serial_Write(char lpOut[1]);

    //关闭串口
	void Serial_Close();
private:
	HANDLE Handle_Comm; //创建串口
};

