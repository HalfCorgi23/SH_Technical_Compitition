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
		float distance;  //测得的距离
		void calculate_distance(Mat source);  //计算距离并显示实时图像
    private:
		Mat source;   //源图像
		Mat canny;  //边缘图像
		Mat theshold;  //阈值图像
		Mat dust;  //膨胀图像
		Mat gauss; //高斯模糊图像
		Mat binary; //二值图像
		vector<vector<Point>> contours; //连通域
		Rect maxRect;
};

#endif