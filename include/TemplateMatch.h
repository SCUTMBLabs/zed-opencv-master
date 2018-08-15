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
	cv::Mat detectWindow;//��ⴰ��
	static cv::Mat image;   //��Ƶ��
	static cv::Mat imageCopy; //���ƾ��ο�ʱ��������ԭͼ��ͼ��
	cv::Mat rectImage;  //��ͼ��
	static bool leftButtonDownFlag; //�����������Ƶ��ͣ���ŵı�־λ
	static cv::Point originalPoint; //���ο����
	static cv::Point processPoint; //���ο��յ�
	static bool gettempl_RIGHT;
	static bool gettempl_LEFT;


	int resultRows;  //ģ��ƥ��result����
	int resultcols;  //ģ��ƥ��result����
	cv::Mat ImageResult;  //ģ��ƥ��result
	double minValue;   //ģ��ƥ��result��Сֵ
	double maxValude;   //ģ��ƥ��result���ֵ
	cv::Point minPoint;   //ģ��ƥ��result��Сֵλ��
	cv::Point maxPoint;    //ģ��ƥ��result���ֵλ��
	
	static std::vector<cv::Mat> Template_batch;
	static int index_template ;
	void static on_Mouse_RIGHT(int event, int x, int y, int, void*);
	void static on_Mouse_LEFT(int event, int x, int y, int, void*);

	cv::Point init_template(int i);
	cv::Point update_template(int i);
	 



};
