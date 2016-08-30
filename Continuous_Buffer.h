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
	//连续稳定缓冲模型
	Continuous_Buffer();
	//双精度浮点型连续稳定缓冲
	double Output(double input);
	//整型连续稳定缓冲
	int Output(int input);
	//浮点型连续稳定缓冲
	float Output(float input);
	//横坐标边界缓冲
	int Horizon_Buffer(int input);
	//纵坐标边界缓冲
	int Vertical_Buffer(int input);
};

