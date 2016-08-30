#include <Windows.h>
#include <iostream>


using namespace std;

#pragma once
/*��������
�������ڣ�Serial_Connect(string)
����0~9����COM1:��
����10�����ϣ���.\\COM10:��
(Ĭ��COM3)
��ʼ�����ڣ�Serial_Init()
дһλ���ڣ�Serial_Write(char)
�رմ��ڣ�Serial_Close()
*/
class Serial_Connect
{
public:
	Serial_Connect(int serial_num);
	~Serial_Connect();

	//��ʼ������
	int Serial_Init();

	//дһλ����
	void Serial_Write(char lpOut[1]);

	/*��һλ����*/
	char Serial_Read();

    //�رմ���
	void Serial_Close();

	//����ͨ��Э��
	void Send_Command(int x, int y);
private:
	HANDLE Handle_Comm; //��������
};

