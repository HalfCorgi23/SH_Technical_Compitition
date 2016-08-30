#include "LaserRador.h"
#include "stdafx.h"

void Laser_Rador::calculate_distance(Mat source)
{
	GaussianBlur(source, gauss, Size(3, 3), 1.5);
	threshold(gauss, theshold, 252, 256, 1);//阈值限制
	cvtColor(theshold, binary, CV_BGR2GRAY);//二值化
	bitwise_not(binary, theshold);//二值反色
	dilate(theshold, dust, Mat(1, 1, CV_8U), Point(-1, -1), 0);//膨胀
	imshow("阈值图像", dust);
	findContours(dust, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //寻找连通域
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
	maxRect = boundingRect(maxContour);//最大连通域矩形
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
	imshow("阈值膨胀图像", source);
}
