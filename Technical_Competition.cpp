// 文件名：Technical_Competition.cpp
// 所属项目：Technical_Competition.sln

#include "stdafx.h"

#include "Transfrom_String.h"
#include "Serial_Connect.h"
#include "Continuous_Buffer.h"

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/ml/ml.hpp>
#include <vector>

#define CAMERA_NUM_0 1
#define CAMERA_NUM_1 0
#define RAD_ANGLE 57.2956

#define THRESHOLD_MAX 40
#define THRESHOLD_MIN 1

#define THRESHOLD_MAX_BOX 130
#define THRESHOLD_MIN_BOX 110

#define THRESHOLD_LASER 252
#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

using namespace std;
using namespace cv;
using namespace ml;

Transfrom_String trans_str;//各种转string
Serial_Connect serial_conn_0(4);//声明梯形板串口
Serial_Connect serial_conn_1(6);//声明Arduino板串口

Continuous_Buffer buffer_angle;//角度连续缓冲区
Continuous_Buffer buffer_edge;//边界连续缓冲区

int mlp_array[] = {48,48,48};//神经网络数组

Mat source;   //源图像
Mat canny;  //边缘图像
Mat theshold;  //阈值图像
Mat dust;  //膨胀图像
Mat gauss; //高斯模糊图像
Mat gray; //灰度图像
Mat gray2; //灰度图像2
Mat hsv;//HSV图像
Mat mlp;//神经网络权值矩阵
Mat mlp_layer(1, 3, CV_32S, mlp_array);//神经网络层数
Mat target, target_temp;//神经网络训练模板

Ptr<ANN_MLP> MLP;//神经网络模型
Ptr<TrainData> MLP_TRAIN_DATA;//神经网络训练模型

vector<vector<Point>> contours; //连通域
vector<Vec3f> circles; //圆
CvBox2D ellip;  //拟合的椭圆
vector<cv::Point> maxContour;//最大连通域

Scalar red(0, 0, 255);//红色
Scalar blue(0, 255, 0);//蓝色
Scalar yellow(255, 255, 0);//黄色
Scalar white(255, 255, 255);//白色

Point ui_1(240, 180);//UI左上角
Point ui_2(400, 180);//UI右上角
Point ui_3(400, 300);//UI右下角
Point ui_4(240, 300);//UI左下角

//UI显示，输入（-1,-1），显示白色
void UI_Display(Mat input,int x,int y,string angle,string distance)
{
	string dis_out = "Distance:" + distance + "m";
	string ang_out = "Angle:" + angle +"degree";
	int text_x = 380;
	int text_y_1 = 400;
	int text_y_2 = 420;
	int text_y_3 = 440;
	line(input, Point(630,470), Point(10, 470), white, 1, 8, 0);
	line(input, Point(10, 470), Point(10, 10), white, 1, 8, 0);
	line(input, Point(10, 10), Point(630, 10), white, 1, 8, 0);
	line(input, Point(630, 10), Point(630, 470), white, 1, 8, 0);
	if (x == -1 && y == -1)
	{
		line(input, ui_1, ui_2, white, 2, 8, 0);
		line(input, ui_2, ui_3, white, 2, 8, 0);
		line(input, ui_3, ui_4, white, 2, 8, 0);
		line(input, ui_4, ui_1, white, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "NO TARGET", Point(text_y_1, text_y_1), 2, 1, white, 2, 8, false);
		putText(input, "Distance:-m", Point(text_y_1, text_y_2), 1, 1, white, 2, 8, false);
		putText(input, "Angle:-degree", Point(text_y_1, text_y_3), 1, 1, white, 2, 8, false);
	}
	else if (x <= 240 && y <= 180)
	{
		
		line(input, ui_1, ui_2, yellow, 2, 8, 0);
		line(input, ui_2, ui_3, white, 2, 8, 0);
		line(input, ui_3, ui_4, white, 2, 8, 0);
		line(input, ui_4, ui_1, yellow, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x > 240 && x < 400 && y <= 180)
	{
		line(input, ui_1, ui_2, yellow, 2, 8, 0);
		line(input, ui_2, ui_3, white, 2, 8, 0);
		line(input, ui_3, ui_4, white, 2, 8, 0);
		line(input, ui_4, ui_1, white, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x >= 400 && y <= 180)
	{
		line(input, ui_1, ui_2, yellow, 2, 8, 0);
		line(input, ui_2, ui_3, yellow, 2, 8, 0);
		line(input, ui_3, ui_4, white, 2, 8, 0);
		line(input, ui_4, ui_1, white, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x <= 240 && y > 180 && y < 300)
	{
		line(input, ui_1, ui_2, white, 2, 8, 0);
		line(input, ui_2, ui_3, white, 2, 8, 0);
		line(input, ui_3, ui_4, white, 2, 8, 0);
		line(input, ui_4, ui_1, yellow, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x > 240 && x < 400 && y > 180 && y < 300)
	{
		line(input, ui_1, ui_2, red, 2, 8, 0);
		line(input, ui_2, ui_3, red, 2, 8, 0);
		line(input, ui_3, ui_4, red, 2, 8, 0);
		line(input, ui_4, ui_1, red, 2, 8, 0);
		circle(input, Point(320, 240), 3, red, 3, 8, 0);
		putText(input, "TARGET AIMED", Point(text_x, text_y_1), 2, 1, red, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, red, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, red, 2, 8, false);
	}
	else if (x >= 400 && y > 180 && y < 300)
	{
		line(input, ui_1, ui_2, white, 2, 8, 0);
		line(input, ui_2, ui_3, yellow, 2, 8, 0);
		line(input, ui_3, ui_4, white, 2, 8, 0);
		line(input, ui_4, ui_1, white, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x <= 240 && y >= 300)
	{
		line(input, ui_1, ui_2, white, 2, 8, 0);
		line(input, ui_2, ui_3, white, 2, 8, 0);
		line(input, ui_3, ui_4, yellow, 2, 8, 0);
		line(input, ui_4, ui_1, yellow, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x > 240 && x < 400 && y >= 300)
	{
		line(input, ui_1, ui_2, white, 2, 8, 0);
		line(input, ui_2, ui_3, white, 2, 8, 0);
		line(input, ui_3, ui_4, yellow, 2, 8, 0);
		line(input, ui_4, ui_1, white, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, yellow, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, yellow, 2, 8, false);
	}
	else if (x >= 400 && y >= 300)
	{
		line(input, ui_1, ui_2, white, 2, 8, 0);
		line(input, ui_2, ui_3, yellow, 2, 8, 0);
		line(input, ui_3, ui_4, yellow, 2, 8, 0);
		line(input, ui_4, ui_1, white, 2, 8, 0);
		circle(input, Point(320, 240), 3, white, 3, 8, 0);
		putText(input, "TARGET FOUND", Point(text_x, text_y_1), 2, 1, yellow, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, white, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, white, 2, 8, false);
	}

}

void Serial_Connection(int x, int y)
{
	serial_conn_0.Send_Command(x, y);
}

/*激光测距*/
double Lasor_Rador_Distance(Mat input)  //激光测距
{
	GaussianBlur(input, gauss, Size(3, 3), 1.5);
	threshold(gauss, theshold, THRESHOLD_LASER, 255, CV_THRESH_BINARY_INV);//阈值限制
	cvtColor(theshold, gray, CV_BGR2GRAY);//二值化
	bitwise_not(gray, theshold);//二值反色
	dilate(theshold, dust, Mat(1, 1, CV_8U), Point(-1, -1), 0);//膨胀
	imshow("阈值图像", dust);
	findContours(dust, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //寻找连通域
	double maxArea = 0;//最大连通域面积
	for (size_t i = 0; i < contours.size(); i++)//寻找最大连通域
	{
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	Rect maxRect = boundingRect(maxContour);//最大连通域矩形
	String laserpoint_x = trans_str.int_to_string(maxRect.x);
	String laserpoint_y = trans_str.int_to_string(maxRect.y);
	string out_put = "(" + laserpoint_x + "," + laserpoint_x + ")";
	putText(input, out_put, Point(maxRect.x, maxRect.y), 1, 1, Scalar(255, 0, 255, 255), 2, false);
	double distance_d = 6.0 / tan(0.0007*(maxRect.y - 240) - 0.005);
	string out_distance = trans_str.double_to_string(distance_d);
	out_distance = out_distance + "cm";
	putText(source, out_distance, Point(30, 30), 1, 2, Scalar(255, 255, 0, 255), 2, false);
	imshow("阈值膨胀图像", input);
	return distance_d;
}

/*识别靶心位置*/
bool Target_Find(Mat input)
{
	source = input.clone();
	GaussianBlur(source, gauss, Size(15, 15), 3, 3);
	cvtColor(gauss, gray, CV_BGR2GRAY);//彩色转灰度
	GaussianBlur(gray, gauss, Size(15, 15), 3, 3);
	imshow("灰度图像", gauss);
	threshold(gauss, theshold, THRESHOLD_MAX, 255, CV_THRESH_TOZERO_INV);//阈值限制
	bitwise_not(theshold, gray);//取反色
	threshold(gray, theshold, (255-THRESHOLD_MIN), 255, CV_THRESH_BINARY);//阈值限制
	bitwise_not(theshold, gray);//取反色
	gray2 = gray.clone();
	imshow("反色图像", gray);
	/******椭圆检测*******/
	findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxArea = 0;//最大连通域面积
	vector<cv::Point> maxContour;//最大连通域
	for (size_t i = 0; i < contours.size(); i++)//寻找最大连通域
	{
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	if (maxArea > 36)
	{
		ellip = fitEllipse(maxContour);
		int ellipse_1_x = ellip.center.x;//目标中心x
		int ellipse_1_y = ellip.center.y;//目标形中心y
		float ellipse_1_height = ellip.size.width;//目标长轴
		float ellipse_1_width = ellip.size.height;//目标短轴
		float target_rad = acos(ellip.size.width / ellip.size.height);
		float target_angle = RAD_ANGLE * target_rad - 6.0;
		if (target_angle < 50)
		{
			if (ellipse_1_x > 10 && ellipse_1_x<630 && ellipse_1_y > 10 && ellipse_1_y<470)
			{
				
				////提取目标
				int target_row_min = buffer_edge.Vertical_Buffer(ellip.center.y - ellip.size.height / 2);//目标最大纵坐标
				int target_row_max = buffer_edge.Vertical_Buffer(ellip.center.y + ellip.size.height / 2);//目标最小纵坐标
				int target_col_min = buffer_edge.Horizon_Buffer(ellip.center.x - ellip.size.width / 2);//目标最小横坐标
				int target_col_max = buffer_edge.Horizon_Buffer(ellip.center.x + ellip.size.width / 2);//目标最大横坐标
				buffer_edge.Min_Max_Adjust(target_row_min, target_row_max);
				buffer_edge.Min_Max_Adjust(target_col_min, target_col_max);
				target = gray2.rowRange(target_row_min, target_row_max);
				target_temp = target.colRange(target_col_min, target_col_max);
				bitwise_not(target_temp, target);
				///椭圆验证
				findContours(target, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				maxArea = 0;//最大连通域面积
				maxContour;//最大连通域
				double area;
				for (size_t i = 0; i < contours.size(); i++)//寻找最大连通域
				{
					area = contourArea(contours[i]);
					if (area > maxArea)
					{
						maxArea = area;
						maxContour = contours[i];
					}
				}
				if (area > 36)
				{
					ellip = fitEllipse(maxContour);
					int ellipse_2_x = ellip.center.x;//大矩形中心x
					int ellipse_2_y = ellip.center.y;//大矩形中心y
					int distance_center = (ellipse_2_x - target.cols / 2)*(ellipse_2_x - target.cols / 2) + (ellipse_2_y - target.rows / 2)*(ellipse_2_y - target.rows / 2);
					//circle(source, Point(320, 240), 8, Scalar(0, 0, 255), 3, 8, 0);
					circle(source, Point(320, 240), 3, Scalar(0, 0, 255), 3, 8, 0);
					if (distance_center < 150)
					{
						circle(source, Point(ellipse_1_x, ellipse_1_y), ellipse_1_height / 2, Scalar(0, 255, 0), 2, 8, 0);
						circle(source, Point(ellipse_1_x, ellipse_1_y), ellipse_1_width / 2, Scalar(0, 255, 0), 2, 8, 0);
						string angle_output = trans_str.float_to_string(buffer_angle.Output(target_angle));
						putText(source, angle_output, Point(ellipse_1_x, ellipse_1_y), 1, 1, Scalar(255, 0, 0, 255), 2, false);
						UI_Display(source, ellipse_1_x, ellipse_1_y, angle_output, "-");
						Serial_Connection(ellipse_1_x, ellipse_1_y);
						imshow("椭圆检测", source);
						return true;
					}
					else
					{
						UI_Display(source, -1, -1, "-", "-");
						Serial_Connection(-1, -1);
						imshow("椭圆检测", source);
						return false;
					}
				}
				else
				{
					UI_Display(source, -1, -1, "-", "-");
					Serial_Connection(-1, -1);
					imshow("椭圆检测", source);
					return false;
				}
			}
			else
			{
				UI_Display(source, -1, -1, "-", "-");
				Serial_Connection(-1, -1);
				imshow("椭圆检测", source);
				return false;
			}
		}
		else
		{
			UI_Display(source, -1, -1, "-", "-");
			Serial_Connection(-1, -1);
			imshow("椭圆检测", source);
			return false;
		}
	}
	else
	{
		UI_Display(source, -1, -1, "-", "-");
		Serial_Connection(-1, -1);
		imshow("椭圆检测", source);
		return false;
	}
}

/*弹盒识别*/
int Bullet_Box(Mat input,int sensor_statue)
{
	printf_s("%d", sensor_statue);
	if (sensor_statue == 8)
	{
		source = input.clone();
		GaussianBlur(source, gauss, Size(15, 15), 3, 3);
		cvtColor(gauss, gray, CV_BGR2GRAY);//彩色转灰度
		imshow("弹盒识别4", gray);
		threshold(gray, theshold, THRESHOLD_MAX_BOX, 0, CV_THRESH_TOZERO_INV);//阈值限制
		imshow("弹盒识别2", theshold);
		bitwise_not(theshold, gray);//取反色
		imshow("弹盒识别3", gray);
		threshold(gray, theshold, (255 - THRESHOLD_MIN_BOX), 255, CV_THRESH_BINARY);//阈值限制
		imshow("弹盒识别4", theshold);
		bitwise_not(theshold, gray);//取反色
		gray2 = gray.clone();
		imshow("反色图像", gray);
		findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //寻找连通域
		double maxArea2 = 0;//最大连通域面积
		vector<cv::Point> maxContour;//最大连通域
		Rect Rect_temp;
		for (size_t i = 0; i < contours.size(); i++)//寻找最大连通域
		{
			double area = contourArea(contours[i]);
			if (area > maxArea2)
			{
				Rect_temp=boundingRect(contours[i]);
				if ((Rect_temp.y+ Rect_temp.height/2) >= 160 && (Rect_temp.y + Rect_temp.height / 2) <= 320)
				{
					maxArea2 = area;
					maxContour = contours[i];
				}
			}
		}
		if (maxArea2 > 10000)
		{
			Rect maxRect = boundingRect(maxContour);
			int target_x = maxRect.x+ Rect_temp.width / 2;
			int target_y = maxRect.y+Rect_temp.height / 2;
			circle(source, Point(target_x + Rect_temp.width / 2, target_y + Rect_temp.height / 2), 10, Scalar(255, 0, 0, 255), 2, 8, 0);
			circle(source, Point(target_x - Rect_temp.width / 2, target_y - Rect_temp.height / 2), 10, Scalar(255, 0, 0, 255), 2, 8, 0);
			circle(source, Point(target_x + Rect_temp.width / 2, target_y - Rect_temp.height / 2), 10, Scalar(255, 0, 0, 255), 2, 8, 0);
			circle(source, Point(target_x - Rect_temp.width / 2, target_y + Rect_temp.height / 2), 10, Scalar(255, 0, 0, 255), 2, 8, 0);
			//imshow("弹盒识别", source);
			if (target_x < 160)
			{
				serial_conn_0.Serial_Write("j");//车子可以向左前进
				printf_s("7车子可以向左前进\n");
			}
			else if (target_x > 160)
			{
				serial_conn_0.Serial_Write("k");//车子可以向右前进
				printf_s("7车子可以向右前进\n");
			}
			else
			{
				serial_conn_0.Serial_Write("l");//车子可以继续前进
				printf_s("7车子可以继续前进\n");
			}
		}
		else
		{
			serial_conn_0.Serial_Write("m");//车子随机前进
			printf_s("车子随机前进\n");
		}
	}
	else
	{
		switch (sensor_statue)
		{
		default:
			serial_conn_0.Serial_Write("m");//车子随机前进
			printf_s("%d车子随机前进\n", sensor_statue);
			break;
		case 1:
			serial_conn_0.Serial_Write("l");//传感器左正对目标
			printf_s("1传感器左正对目标\n");
			break;
		case 2:
			serial_conn_0.Serial_Write("e");//传感器左正对目标
			printf_s("2传感器左正对目标\n");
			break;
		case 3:
			serial_conn_0.Serial_Write("f");//传感器右正对目标
			printf_s("3传感器右正对目标\n");
			break;
		case 4:
			serial_conn_0.Serial_Write("g");//目标在传感器左侧
			printf_s("4目标在传感器左侧\n");
			break;
		case 5:
			serial_conn_0.Serial_Write("h");//目标在传感器右侧
			printf_s("5目标在传感器右侧\n");
			break;
		case 6:
			serial_conn_0.Serial_Write("i");//车在弹箱角处
			printf_s("6车在弹箱角处\n");
			break;
		}
	}
	imshow("弹盒识别", source);
	return 0;
}

//主函数入口
int main()
{
	char switch_0;//摄像头检测开关
	VideoCapture capture(CAMERA_NUM_1);  //摄像头1采集图像
	VideoCapture capture2(CAMERA_NUM_0);  //摄像头2采集图像

	if (!capture.isOpened())
	{
		printf_s("1号摄像头连接失败！\n");
		scanf_s(&switch_0);
		return -1;
	}
	else
	{
		printf_s("1号摄像头连接成功！\n");
	}
	if (!capture2.isOpened())
	{
		printf_s("2号摄像头连接失败！\n");
		scanf_s(&switch_0);
		return -1;
	}
	else
	{
		printf_s("2号摄像头连接成功！\n");
	}

	bool stop = false;

	for (int i = 0; i < 30; i++)
	{
		capture >> source;
		capture2 >> source;
		waitKey(33);
	}

	int serial=serial_conn_0.Serial_Init();//初始化梯形板串口
	if (serial == 0)
	{
		printf_s("梯形板串口打开成功！\n");
		
	}
	else
	{
		printf_s("梯形板串口打开失败！\n");
		scanf_s(&switch_0);
	}

	serial = serial_conn_1.Serial_Init();//初始化Arduino串口
	if (serial == 0)
	{
		printf_s("Arduino板串口打开成功！\n");

	}
	else
	{
		printf_s("Arduino板串口打开失败！\n");
		scanf_s(&switch_0);
	}

	int statue;//传感器状态

	while (!stop)
	{
		/*statue = trans_str.char_to_int(serial_conn_1.Serial_Read());
		if (statue == 0)
		{
			capture >> source;
			Target_Find(source);
		}
		else
		{*/
			capture2 >> source;
			//Bullet_Box(source, statue);
			Bullet_Box(source, 8);
		/*}*/
		waitKey(33);
	}

	serial_conn_0.Serial_Close();
	serial_conn_1.Serial_Close();
	printf_s("程序即将退出！\n");

	return 0;
}




