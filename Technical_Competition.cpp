// �ļ�����Technical_Competition.cpp
// ������Ŀ��Technical_Competition.sln

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

Transfrom_String trans_str;//����תstring
Serial_Connect serial_conn;//��������

Continuous_Buffer buffer_angle;//�Ƕ�����������
Continuous_Buffer buffer_edge;//�߽�����������

int mlp_array[] = {48,48,48};//����������

Mat source;   //Դͼ��
Mat canny;  //��Եͼ��
Mat theshold;  //��ֵͼ��
Mat dust;  //����ͼ��
Mat gauss; //��˹ģ��ͼ��
Mat gray; //�Ҷ�ͼ��
Mat gray2; //�Ҷ�ͼ��2
Mat hsv;//HSVͼ��
Mat mlp;//������Ȩֵ����
Mat mlp_layer(1, 3, CV_32S, mlp_array);//���������
Mat target, target_temp;//������ѵ��ģ��

Ptr<ANN_MLP> MLP;//������ģ��
Ptr<TrainData> MLP_TRAIN_DATA;//������ѵ��ģ��

vector<vector<Point>> contours; //��ͨ��
vector<Vec3f> circles; //Բ
CvBox2D ellip;  //��ϵ���Բ
vector<cv::Point> maxContour;//�����ͨ��

Scalar red(0, 0, 255);//��ɫ
Scalar blue(0, 255, 0);//��ɫ
Scalar yellow(255, 255, 0);//��ɫ
Scalar white(255, 255, 255);//��ɫ

Point ui_1(240, 180);//UI���Ͻ�
Point ui_2(400, 180);//UI���Ͻ�
Point ui_3(400, 300);//UI���½�
Point ui_4(240, 300);//UI���½�

//UI��ʾ�����루-1,-1������ʾ��ɫ
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
	else if (x > 240 && x < 400 && y > 180 && y < 300)//����׼
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

/*������*/
double Lasor_Rador_Distance(Mat input)  //������
{
	GaussianBlur(input, gauss, Size(3, 3), 1.5);
	threshold(gauss, theshold, THRESHOLD_LASER, 255, CV_THRESH_BINARY_INV);//��ֵ����
	cvtColor(theshold, gray, CV_BGR2GRAY);//��ֵ��
	bitwise_not(gray, theshold);//��ֵ��ɫ
	dilate(theshold, dust, Mat(1, 1, CV_8U), Point(-1, -1), 0);//����
	imshow("��ֵͼ��", dust);
	findContours(dust, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //Ѱ����ͨ��
	double maxArea = 0;//�����ͨ�����
	for (size_t i = 0; i < contours.size(); i++)//Ѱ�������ͨ��
	{
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	Rect maxRect = boundingRect(maxContour);//�����ͨ�����
	String laserpoint_x = trans_str.int_to_string(maxRect.x);
	String laserpoint_y = trans_str.int_to_string(maxRect.y);
	string out_put = "(" + laserpoint_x + "," + laserpoint_x + ")";
	putText(input, out_put, Point(maxRect.x, maxRect.y), 1, 1, Scalar(255, 0, 255, 255), 2, false);
	double distance_d = 6.0 / tan(0.0007*(maxRect.y - 240) - 0.005);
	string out_distance = trans_str.double_to_string(distance_d);
	out_distance = out_distance + "cm";
	putText(source, out_distance, Point(30, 30), 1, 2, Scalar(255, 255, 0, 255), 2, false);
	imshow("��ֵ����ͼ��", input);
	return distance_d;
}

/*ʶ�����λ��*/
bool Target_Find(Mat input)
{
	source = input.clone();
	GaussianBlur(source, gauss, Size(15, 15), 3, 3);
	cvtColor(gauss, gray, CV_BGR2GRAY);//��ɫת�Ҷ�
	GaussianBlur(gray, gauss, Size(15, 15), 3, 3);
	threshold(gauss, theshold, THRESHOLD_MAX, 255, CV_THRESH_TOZERO_INV);//��ֵ����
	bitwise_not(theshold, gray);//ȡ��ɫ
	threshold(gray, theshold, (255-THRESHOLD_MIN), 255, CV_THRESH_BINARY);//��ֵ����
	bitwise_not(theshold, gray);//ȡ��ɫ
	gray2 = gray.clone();
	imshow("��ɫͼ��", gray);
	/******��Բ���*******/
	findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxArea = 0;//�����ͨ�����
	vector<cv::Point> maxContour;//�����ͨ��
	for (size_t i = 0; i < contours.size(); i++)//Ѱ�������ͨ��
	{
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	ellip = fitEllipse(maxContour);
	int ellipse_1_x = ellip.center.x;//Ŀ������x
	int ellipse_1_y = ellip.center.y;//Ŀ��������y
	float ellipse_1_height = ellip.size.width;//Ŀ�곤��
	float ellipse_1_width = ellip.size.height;//Ŀ�곤��
	float target_rad = acos(ellip.size.width / ellip.size.height);
	float target_angle = RAD_ANGLE * target_rad-6.0;
	////��ȡĿ��
	int target_row_min = buffer_edge.Vertical_Buffer(ellip.center.y - ellip.size.height / 2 + 5);//Ŀ�����������
	int target_row_max = buffer_edge.Vertical_Buffer(ellip.center.y + ellip.size.height / 2 - 5);//Ŀ����С������
	int target_col_min = buffer_edge.Horizon_Buffer(ellip.center.x - ellip.size.width / 2 + 5);//Ŀ����С������
	int target_col_max = buffer_edge.Horizon_Buffer(ellip.center.x + ellip.size.width / 2 - 5);//Ŀ����������
	target = gray2.rowRange(target_row_min, target_row_max);
	target_temp = target.colRange(target_col_min, target_col_max);
	bitwise_not(target_temp, target);
	///��Բ��֤
	findContours(target, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	maxArea = 0;//�����ͨ�����
	maxContour;//�����ͨ��
	for (size_t i = 0; i < contours.size(); i++)//Ѱ�������ͨ��
	{
		double area = contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	ellip = fitEllipse(maxContour);
	int ellipse_2_x = ellip.center.x;//���������x
	int ellipse_2_y = ellip.center.y;//���������y
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
		imshow("��Բ���", source);
		return true;
	}
	
	else
	{
		UI_Display(source, -1, -1, "-", "-");
		imshow("��Բ���", source);
		return false;
	}
}

int main()
{
	VideoCapture capture(CAMERA_NUM);  //����ͷ�ɼ�ͼ��
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
	//imshow("��������", mlp);

	serial_conn.Serial_Init();//��ʼ������

	while (!stop)
	{
		capture >> source;
		//imshow("ԭʼͼ��", source);
		Target_Find(source);
		waitKey(10);
	}
	serial_conn.Serial_Close();
}




