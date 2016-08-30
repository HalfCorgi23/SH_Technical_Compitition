#ifndef LASERRADOR_H
#define LASERRADOR_H

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>  
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <sstream>

using namespace cv;
using namespace std;

class Laser_Rador
{
    public:
		float distance;  //��õľ���
		void calculate_distance(Mat source);  //������벢��ʾʵʱͼ��
    private:
		Mat source;   //Դͼ��
		Mat canny;  //��Եͼ��
		Mat theshold;  //��ֵͼ��
		Mat dust;  //����ͼ��
		Mat gauss; //��˹ģ��ͼ��
		Mat binary; //��ֵͼ��
		vector<vector<Point>> contours; //��ͨ��
		Rect maxRect;
};

#endif