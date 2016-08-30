#pragma once
#define INT_BUFFER 0
#define FLOAT_BUFFER 1
#define DOUBLE_BUFFER 2
#define MAT_SIZE_HORIZON 640
#define MAT_SIZE_VERTICAL 480
#define BUFFER_SIZE 100000


class Continuous_Buffer
{
public:
	//�����ȶ�����ģ��
	Continuous_Buffer();
	//˫���ȸ����������ȶ�����
	double Output(double input);
	//���������ȶ�����
	int Output(int input);
	//�����������ȶ�����
	float Output(float input);
	//������߽绺��
	int Horizon_Buffer(int input);
	//������߽绺��
	int Vertical_Buffer(int input);
};

