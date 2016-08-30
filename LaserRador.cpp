#include "LaserRador.h"
#include "stdafx.h"

void Laser_Rador::calculate_distance(Mat source)
{
	GaussianBlur(source, gauss, Size(3, 3), 1.5);
	threshold(gauss, theshold, 252, 256, 1);//��ֵ����
	cvtColor(theshold, binary, CV_BGR2GRAY);//��ֵ��
	bitwise_not(binary, theshold);//��ֵ��ɫ
	dilate(theshold, dust, Mat(1, 1, CV_8U), Point(-1, -1), 0);//����
	imshow("��ֵͼ��", dust);
	findContours(dust, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //Ѱ����ͨ��
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
	maxRect = boundingRect(maxContour);//�����ͨ�����
	char temp[8];
	_itoa_s(maxRect.x, temp, 10);
	String laserpoint_x = temp;
	_itoa_s(maxRect.y, temp, 10);
	String laserpoint_y = temp;
	string out_put = "(" + laserpoint_x + "," + laserpoint_x + ")";
	putText(source, out_put, Point(maxRect.x, maxRect.y), 1, 1, Scalar(255, 255, 255, 255), 2, false);
	distance = 6.0 / tan(0.0007*(maxRect.y - 240) - 0.005);
	string out_distance;
	char distance_temp[20];
	sprintf_s(distance_temp, "%f", distance);
	out_distance = distance_temp;
	out_distance = out_distance + "cm";
	putText(source, out_distance, Point(30, 30), 1, 2, Scalar(255, 255, 255, 255), 2, false);
	imshow("��ֵ����ͼ��", source);
}
