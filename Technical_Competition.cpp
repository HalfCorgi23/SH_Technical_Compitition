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

Transfrom_String trans_str;//����תstring
Serial_Connect serial_conn_0(4);//�������ΰ崮��
Serial_Connect serial_conn_1(6);//����Arduino�崮��

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
	imshow("�Ҷ�ͼ��", gauss);
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
	if (maxArea > 36)
	{
		ellip = fitEllipse(maxContour);
		int ellipse_1_x = ellip.center.x;//Ŀ������x
		int ellipse_1_y = ellip.center.y;//Ŀ��������y
		float ellipse_1_height = ellip.size.width;//Ŀ�곤��
		float ellipse_1_width = ellip.size.height;//Ŀ�����
		float target_rad = acos(ellip.size.width / ellip.size.height);
		float target_angle = RAD_ANGLE * target_rad - 6.0;
		if (target_angle < 50)
		{
			if (ellipse_1_x > 10 && ellipse_1_x<630 && ellipse_1_y > 10 && ellipse_1_y<470)
			{
				
				////��ȡĿ��
				int target_row_min = buffer_edge.Vertical_Buffer(ellip.center.y - ellip.size.height / 2);//Ŀ�����������
				int target_row_max = buffer_edge.Vertical_Buffer(ellip.center.y + ellip.size.height / 2);//Ŀ����С������
				int target_col_min = buffer_edge.Horizon_Buffer(ellip.center.x - ellip.size.width / 2);//Ŀ����С������
				int target_col_max = buffer_edge.Horizon_Buffer(ellip.center.x + ellip.size.width / 2);//Ŀ����������
				buffer_edge.Min_Max_Adjust(target_row_min, target_row_max);
				buffer_edge.Min_Max_Adjust(target_col_min, target_col_max);
				target = gray2.rowRange(target_row_min, target_row_max);
				target_temp = target.colRange(target_col_min, target_col_max);
				bitwise_not(target_temp, target);
				///��Բ��֤
				findContours(target, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				maxArea = 0;//�����ͨ�����
				maxContour;//�����ͨ��
				double area;
				for (size_t i = 0; i < contours.size(); i++)//Ѱ�������ͨ��
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
					int ellipse_2_x = ellip.center.x;//���������x
					int ellipse_2_y = ellip.center.y;//���������y
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
						imshow("��Բ���", source);
						return true;
					}
					else
					{
						UI_Display(source, -1, -1, "-", "-");
						Serial_Connection(-1, -1);
						imshow("��Բ���", source);
						return false;
					}
				}
				else
				{
					UI_Display(source, -1, -1, "-", "-");
					Serial_Connection(-1, -1);
					imshow("��Բ���", source);
					return false;
				}
			}
			else
			{
				UI_Display(source, -1, -1, "-", "-");
				Serial_Connection(-1, -1);
				imshow("��Բ���", source);
				return false;
			}
		}
		else
		{
			UI_Display(source, -1, -1, "-", "-");
			Serial_Connection(-1, -1);
			imshow("��Բ���", source);
			return false;
		}
	}
	else
	{
		UI_Display(source, -1, -1, "-", "-");
		Serial_Connection(-1, -1);
		imshow("��Բ���", source);
		return false;
	}
}

/*����ʶ��*/
int Bullet_Box(Mat input,int sensor_statue)
{
	printf_s("%d", sensor_statue);
	if (sensor_statue == 8)
	{
		source = input.clone();
		GaussianBlur(source, gauss, Size(15, 15), 3, 3);
		cvtColor(gauss, gray, CV_BGR2GRAY);//��ɫת�Ҷ�
		imshow("����ʶ��4", gray);
		threshold(gray, theshold, THRESHOLD_MAX_BOX, 0, CV_THRESH_TOZERO_INV);//��ֵ����
		imshow("����ʶ��2", theshold);
		bitwise_not(theshold, gray);//ȡ��ɫ
		imshow("����ʶ��3", gray);
		threshold(gray, theshold, (255 - THRESHOLD_MIN_BOX), 255, CV_THRESH_BINARY);//��ֵ����
		imshow("����ʶ��4", theshold);
		bitwise_not(theshold, gray);//ȡ��ɫ
		gray2 = gray.clone();
		imshow("��ɫͼ��", gray);
		findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //Ѱ����ͨ��
		double maxArea2 = 0;//�����ͨ�����
		vector<cv::Point> maxContour;//�����ͨ��
		Rect Rect_temp;
		for (size_t i = 0; i < contours.size(); i++)//Ѱ�������ͨ��
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
			//imshow("����ʶ��", source);
			if (target_x < 160)
			{
				serial_conn_0.Serial_Write("j");//���ӿ�������ǰ��
				printf_s("7���ӿ�������ǰ��\n");
			}
			else if (target_x > 160)
			{
				serial_conn_0.Serial_Write("k");//���ӿ�������ǰ��
				printf_s("7���ӿ�������ǰ��\n");
			}
			else
			{
				serial_conn_0.Serial_Write("l");//���ӿ��Լ���ǰ��
				printf_s("7���ӿ��Լ���ǰ��\n");
			}
		}
		else
		{
			serial_conn_0.Serial_Write("m");//�������ǰ��
			printf_s("�������ǰ��\n");
		}
	}
	else
	{
		switch (sensor_statue)
		{
		default:
			serial_conn_0.Serial_Write("m");//�������ǰ��
			printf_s("%d�������ǰ��\n", sensor_statue);
			break;
		case 1:
			serial_conn_0.Serial_Write("l");//������������Ŀ��
			printf_s("1������������Ŀ��\n");
			break;
		case 2:
			serial_conn_0.Serial_Write("e");//������������Ŀ��
			printf_s("2������������Ŀ��\n");
			break;
		case 3:
			serial_conn_0.Serial_Write("f");//������������Ŀ��
			printf_s("3������������Ŀ��\n");
			break;
		case 4:
			serial_conn_0.Serial_Write("g");//Ŀ���ڴ��������
			printf_s("4Ŀ���ڴ��������\n");
			break;
		case 5:
			serial_conn_0.Serial_Write("h");//Ŀ���ڴ������Ҳ�
			printf_s("5Ŀ���ڴ������Ҳ�\n");
			break;
		case 6:
			serial_conn_0.Serial_Write("i");//���ڵ���Ǵ�
			printf_s("6���ڵ���Ǵ�\n");
			break;
		}
	}
	imshow("����ʶ��", source);
	return 0;
}

//���������
int main()
{
	char switch_0;//����ͷ��⿪��
	VideoCapture capture(CAMERA_NUM_1);  //����ͷ1�ɼ�ͼ��
	VideoCapture capture2(CAMERA_NUM_0);  //����ͷ2�ɼ�ͼ��

	if (!capture.isOpened())
	{
		printf_s("1������ͷ����ʧ�ܣ�\n");
		scanf_s(&switch_0);
		return -1;
	}
	else
	{
		printf_s("1������ͷ���ӳɹ���\n");
	}
	if (!capture2.isOpened())
	{
		printf_s("2������ͷ����ʧ�ܣ�\n");
		scanf_s(&switch_0);
		return -1;
	}
	else
	{
		printf_s("2������ͷ���ӳɹ���\n");
	}

	bool stop = false;

	for (int i = 0; i < 30; i++)
	{
		capture >> source;
		capture2 >> source;
		waitKey(33);
	}

	int serial=serial_conn_0.Serial_Init();//��ʼ�����ΰ崮��
	if (serial == 0)
	{
		printf_s("���ΰ崮�ڴ򿪳ɹ���\n");
		
	}
	else
	{
		printf_s("���ΰ崮�ڴ�ʧ�ܣ�\n");
		scanf_s(&switch_0);
	}

	serial = serial_conn_1.Serial_Init();//��ʼ��Arduino����
	if (serial == 0)
	{
		printf_s("Arduino�崮�ڴ򿪳ɹ���\n");

	}
	else
	{
		printf_s("Arduino�崮�ڴ�ʧ�ܣ�\n");
		scanf_s(&switch_0);
	}

	int statue;//������״̬

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
	printf_s("���򼴽��˳���\n");

	return 0;
}




