#pragma once
#include <opencv2/opencv.hpp>
#include<iostream>
#include<vector>

#include<cmath>

#include<windows.h>

class TemplateMatch
{
	void ColorThresholding(cv::Mat img_copy);
public:
 
	cv::Point detectWindowPosition;
	cv::Point minPoint;   //模板匹配result最小值位置
	cv::Point maxPoint;    //模板匹配result最大值位置
	cv::Point init_template(int i);
	cv::Point update_template(int i);
	static cv::Point start_point[2][6]; //初始帧模板中点坐标
	static cv::Point originalPoint; //矩形框起点
	static cv::Point processPoint; //矩形框终点

	cv::Mat detectWindow;//检测窗口
	static cv::Mat image;   //视频流
	static cv::Mat imageCopy; //绘制矩形框时用来拷贝原图的图像
	cv::Mat rectImage;  //子图像
	cv::Mat ImageResult;  //模板匹配result
	static cv::Mat image_l;
	static cv::Mat image_r;
	static bool leftButtonDownFlag; //左键单击后视频暂停播放的标志位

	bool getmomentpointEx = false;
	static bool gettempl_RIGHT;
	static bool gettempl_LEFT;
	bool auto_templ_left = false;
	bool auto_templ_right = false;
	static bool getColors;

	double minValue;   //模板匹配result最小值
	double maxValude;   //模板匹配result最大值
	
	static std::vector<cv::Mat> Template_batch;

	int resultRows;  //模板匹配result的行
	int resultcols;  //模板匹配result的列
	int threshold = 100;
	static int index_template ;
	static int CorlorsChosen[3];//采色结果,私有变量，测试公有


	void static on_Mouse_RIGHT(int event, int x, int y, int, void*);
	void static on_Mouse_LEFT(int event, int x, int y, int, void*);
	static void Mouse_getColor(int event, int x, int y, int, void*);
	void auto_Template(int picture);// use color to get the first Template
	

};
