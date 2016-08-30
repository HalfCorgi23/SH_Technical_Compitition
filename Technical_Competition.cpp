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

#define CAMERA_NUM 1
#define RAD_ANGLE 57.2956
#define THRESHOLD_MAX 40
#define THRESHOLD_MIN 10
#define THRESHOLD_LASER 252
#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

using namespace std;
using namespace cv;
using namespace ml;

Transfrom_String trans_str;//各种转string
Serial_Connect serial_conn;//声明串口

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
		serial_conn.Serial_Write("1");
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
		serial_conn.Serial_Write("1");
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
		serial_conn.Serial_Write("1");
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
		serial_conn.Serial_Write("1");
	}
	else if (x > 240 && x < 400 && y > 180 && y < 300)//已瞄准
	{
		line(input, ui_1, ui_2, red, 2, 8, 0);
		line(input, ui_2, ui_3, red, 2, 8, 0);
		line(input, ui_3, ui_4, red, 2, 8, 0);
		line(input, ui_4, ui_1, red, 2, 8, 0);
		circle(input, Point(320, 240), 3, red, 3, 8, 0);
		putText(input, "TARGET AIMED", Point(text_x, text_y_1), 2, 1, red, 2, 8, false);
		putText(input, dis_out, Point(text_x, text_y_2), 1, 1, red, 2, 8, false);
		putText(input, ang_out, Point(text_x, text_y_3), 1, 1, red, 2, 8, false);
		serial_conn.Serial_Write("2");
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
		serial_conn.Serial_Write("1");
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
		serial_conn.Serial_Write("1");
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
		serial_conn.Serial_Write("1");
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
		serial_conn.Serial_Write("1");
	}

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
	ellip = fitEllipse(maxContour);
	int ellipse_1_x = ellip.center.x;//目标中心x
	int ellipse_1_y = ellip.center.y;//目标形中心y
	float ellipse_1_height = ellip.size.width;//目标长轴
	float ellipse_1_width = ellip.size.height;//目标长轴
	float target_rad = acos(ellip.size.width / ellip.size.height);
	float target_angle = RAD_ANGLE * target_rad-6.0;
	////提取目标
	int target_row_min = buffer_edge.Vertical_Buffer(ellip.center.y - ellip.size.height / 2 + 5);//目标最大纵坐标
	int target_row_max = buffer_edge.Vertical_Buffer(ellip.center.y + ellip.size.height / 2 - 5);//目标最小纵坐标
	int target_col_min = buffer_edge.Horizon_Buffer(ellip.center.x - ellip.size.width / 2 + 5);//目标最小横坐标
	int target_col_max = buffer_edge.Horizon_Buffer(ellip.center.x + ellip.size.width / 2 - 5);//目标最大横坐标
	target = gray2.rowRange(target_row_min, target_row_max);
	target_temp = target.colRange(target_col_min, target_col_max);
	bitwise_not(target_temp, target);
	///椭圆验证
	findContours(target, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	maxArea = 0;//最大连通域面积
	maxContour;//最大连通域
	for (size_t i = 0; i < contours.size(); i++)//寻找最大连通域
	{
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	ellip = fitEllipse(maxContour);
	int ellipse_2_x = ellip.center.x;//大矩形中心x
	int ellipse_2_y = ellip.center.y;//大矩形中心y
	int distance_center = (ellipse_2_x - target.cols / 2)*(ellipse_2_x - target.cols / 2) + (ellipse_2_y - target.rows / 2)*(ellipse_2_y - target.rows / 2);
	//circle(source, Point(320, 240), 8, Scalar(0, 0, 255), 3, 8, 0);
	circle(source, Point(320, 240), 3, Scalar(0, 0, 255), 3, 8, 0);
	if (distance_center < 100)
	{
		circle(source, Point(ellipse_1_x, ellipse_1_y), ellipse_1_height / 2, Scalar(0, 255, 0), 2, 8, 0);
		circle(source, Point(ellipse_1_x, ellipse_1_y), ellipse_1_width / 2, Scalar(0, 255, 0), 2, 8, 0);
		string angle_output = trans_str.float_to_string(buffer_angle.Output(target_angle));
		putText(source, angle_output, Point(ellipse_1_x, ellipse_1_y), 1, 1, Scalar(255, 0, 0, 255), 2, false);
		UI_Display(source, ellipse_1_x, ellipse_1_y, angle_output, "-");
		imshow("椭圆检测", source);
		return true;
	}
	
	else
	{
		UI_Display(source, -1, -1, "-", "-");
		imshow("椭圆检测", source);
		return false;
	}
}

int main()
{
	VideoCapture capture(CAMERA_NUM);  //摄像头采集图像
	if (!capture.isOpened())
	{
		return -1;
	}

	bool stop = false;

	capture >> source;
	waitKey(100);
	capture >> source;
	waitKey(100);
	Target_Find(source);

	//FileStorage MLP_File_Read("MLP\\MLP.xml", FileStorage::READ);
	//MLP_File_Read["mlp_mat"] >> mlp;
	//MLP_File_Read.release();
	//imshow("特征矩阵", mlp);

	serial_conn.Serial_Init();//初始化串口

	while (!stop)
	{
		capture >> source;
		//imshow("原始图像", source);
		Target_Find(source);
		waitKey(10);
	}
	serial_conn.Serial_Close();
}




