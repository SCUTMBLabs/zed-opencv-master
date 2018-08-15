#pragma once
#include <opencv2/opencv.hpp>

#include<iostream>

#include<vector>

#include<cmath>

#include<windows.h>

class TemplateMatch
{

public:
 
	cv::Point detectWindowPosition;
	cv::Mat detectWindow;//检测窗口
	static cv::Mat image;   //视频流
	static cv::Mat imageCopy; //绘制矩形框时用来拷贝原图的图像
	cv::Mat rectImage;  //子图像
	static bool leftButtonDownFlag; //左键单击后视频暂停播放的标志位
	static cv::Point originalPoint; //矩形框起点
	static cv::Point processPoint; //矩形框终点
	static bool gettempl_RIGHT;
	static bool gettempl_LEFT;


	int resultRows;  //模板匹配result的行
	int resultcols;  //模板匹配result的列
	cv::Mat ImageResult;  //模板匹配result
	double minValue;   //模板匹配result最小值
	double maxValude;   //模板匹配result最大值
	cv::Point minPoint;   //模板匹配result最小值位置
	cv::Point maxPoint;    //模板匹配result最大值位置
	
	static std::vector<cv::Mat> Template_batch;
	static int index_template ;
	void static on_Mouse_RIGHT(int event, int x, int y, int, void*);
	void static on_Mouse_LEFT(int event, int x, int y, int, void*);

	cv::Point init_template(int i);
	cv::Point update_template(int i);
	 



};
