#pragma once

#include <opencv2/opencv.hpp>

#include<iostream>

#include<vector>

#include<cmath>

#include<windows.h>

class operation
{
	void ColorTheresholding();

	static int CorlorsChosen[4];//��ɫ���,˽�б��������Թ���

	bool getmomentpointEx=false;
	
	cv::Point speed;


public:
	operation();
	~operation();
	//һ�㺯��������thisָ����봫�Σ�ʹ�ú���������ƥ�䣻ʹ�þ�̬���������ڱ��ַ�װ��
	static void Mouse_getColor(int event, int x, int y, int, void*);
	void getContoursAndMoment();
	//void patternMatch();
	
	static cv::Mat image;//ÿһ֡��ͼƬ
	static bool getColors;
	cv::Mat detectWindow;//��ⴰ��
	cv::Point momentpoints[6];
	cv::Point detectWindowPosition;
	int threshold = 100;
};

