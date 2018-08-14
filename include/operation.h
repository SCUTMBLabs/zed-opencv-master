#pragma once

#include <opencv2/opencv.hpp>

#include<iostream>

#include<vector>

#include<cmath>

#include<windows.h>

class operation
{
	void ColorTheresholding();

	static int CorlorsChosen[4];//采色结果,私有变量，测试公有

	bool getmomentpointEx=false;
	
	cv::Point speed;


public:
	operation();
	~operation();
	//一般函数隐含的this指针参与传参，使得函数参数不匹配；使用静态函数在类内保持封装性
	static void Mouse_getColor(int event, int x, int y, int, void*);
	void getContoursAndMoment();
	//void patternMatch();
	
	static cv::Mat image;//每一帧的图片
	static bool getColors;
	cv::Mat detectWindow;//检测窗口
	cv::Point momentpoints[6];
	cv::Point detectWindowPosition;
	int threshold = 100;
};

