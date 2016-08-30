#include "stdafx.h"
#include "Continuous_Buffer.h"

double Temp_d[BUFFER_SIZE]; //˫���ȸ����ͻ�����
int Temp_i[BUFFER_SIZE];  //���ͻ�����
float Temp_f[BUFFER_SIZE];  //�����ͻ�����

bool First_Input; //�״�������

//��������ģ��
Continuous_Buffer::Continuous_Buffer()
{
	First_Input = true;
}

//˫���ȸ�������������
double Continuous_Buffer::Output(double input)
{
	int i = 0;
	if (First_Input)
	{
		for (; i < BUFFER_SIZE; i++)
		{
			Temp_d[i] = input;
		}
		First_Input = false;
		return input;
	}
	else
	{
		for (i = 0; i < (BUFFER_SIZE-1); i++)
		{
			double temp = Temp_d[i];
			Temp_d[i+1] = temp;
		}
		Temp_d[0] = input;
		double s = 0;
		for (i = 0; i < BUFFER_SIZE; i++)
		{
			s += Temp_d[i];
		}
		double output = s / BUFFER_SIZE;
		return output;
	}
}

//������������
int Continuous_Buffer::Output(int input)
{
	double input_double = input;
	int i = 0;
	if (First_Input)
	{
		for (; i < BUFFER_SIZE; i++)
		{
			Temp_d[i] = input_double;
		}
		First_Input = false;
		return input_double;
	}
	else
	{
		for (i = 0; i < (BUFFER_SIZE-1); i++)
		{
			double temp = Temp_d[i];
			Temp_d[i + 1] = temp;
		}
		Temp_d[0] = input_double;
		double s = 0;
		for (i = 0; i < BUFFER_SIZE; i++)
		{
			s += Temp_d[i];
		}
		double output = s / BUFFER_SIZE;
		return output;
	}
}

//��������������
float Continuous_Buffer::Output(float input)
{
	double input_double = input;
	int i = 0;
	if (First_Input)
	{
		for (; i < BUFFER_SIZE; i++)
		{
			Temp_d[i] = input_double;
		}
		First_Input = false;
		return input_double;
	}
	else
	{
		for (i = 0; i < (BUFFER_SIZE-1); i++)
		{
			double temp = Temp_d[i];
			Temp_d[i + 1] = temp;
		}
		Temp_d[0] = input_double;
		double s = 0;
		for (i = 0; i < BUFFER_SIZE; i++)
		{
			s += Temp_d[i];
		}
		double output = s / BUFFER_SIZE;
		return output;
	}
}

int Continuous_Buffer::Horizon_Buffer(int input)
{
	if (input > MAT_SIZE_HORIZON)
	{
		return MAT_SIZE_HORIZON;
	}
	else if(input < 0)
	{
		return 1;
	}
	else
	{
		return input;
	}
}

int Continuous_Buffer::Vertical_Buffer(int input)
{
	if (input > MAT_SIZE_VERTICAL)
	{
		return MAT_SIZE_VERTICAL;
	}
	else if (input < 0)
	{
		return 1;
	}
	else
	{
		return input;
	}
}

void Continuous_Buffer::Min_Max_Adjust(int min, int max)
{
	int temp;
	if (min > max)
	{
		temp = max;
		max = min;
		min = temp;
	}
}
