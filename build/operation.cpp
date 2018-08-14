#include "operation.h"



operation::operation()
{
}


operation::~operation()
{
}


void operation::Mouse_getColor(int event, int x, int y, int, void*)
{
	static cv::Point origin;
	static cv::Rect selection;//静态变量用以开辟内存？
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = cv::Point(x, y);
		break;
	case CV_EVENT_LBUTTONUP:
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = abs(x - origin.x);
		selection.height = abs(y - origin.y);
		getColors = true;
		std::cout << "Corlor area has been selected!" << std::endl;
		std::cout << "Colors have been selected" << std::endl;
		
		int SumOfChannelOneColor = 0, SumOfChannelTwoColor = 0;
		int SumOfChannelThreeColor = 0, SumOfChannelFourColor = 0;
		for (int j = selection.y; j < selection.y + selection.height; j++)
		{
			uchar*data = image.ptr<uchar>(j);
			for (int i = selection.x; i < selection.x + selection.width; i++)
			{
				SumOfChannelOneColor += data[i * 4];
				SumOfChannelTwoColor += data[i * 4 + 1];
				SumOfChannelThreeColor += data[i * 4 + 2];
				//SumOfChannelFourColor += data[i * 4 + 3];
			}
		}
		int SumOfPixels = selection.width*selection.height;
		CorlorsChosen[0] = static_cast<int>(SumOfChannelOneColor / SumOfPixels);
		CorlorsChosen[1] = static_cast<int>(SumOfChannelTwoColor / SumOfPixels);
		CorlorsChosen[2] = static_cast<int>(SumOfChannelThreeColor / SumOfPixels);
		//CorlorsChosen[3] = static_cast<int>(SumOfChannelFourColor / SumOfPixels);

		std::cout << "blue green red: " << CorlorsChosen[0] << " " << CorlorsChosen[1] << " " << CorlorsChosen[2] << std::endl;
	}
}

//静态函数类内声明，需要类外定义开辟内存
cv::Mat operation:: image;//每一帧的图片
int operation::CorlorsChosen[4];//采色结果
bool operation::getColors = false;

void operation::ColorTheresholding()
{
	for (int j = 0; j < detectWindow.rows; j++)
	{
		uchar*data = detectWindow.ptr<uchar>(j);
		for (int i = 0; i< detectWindow.cols; i++)
		{
			
			if ((abs(data[4 * i] - CorlorsChosen[0]) + abs(data[4 * i + 1] - CorlorsChosen[1]) + abs(data[4 * i + 2] - CorlorsChosen[2]))< threshold)
			{
				data[4 * i] = data[4 * i + 1] = data[4 * i + 2] = 255;
			}
			else data[4 * i] = data[4 * i + 1] = data[4 * i + 2] = 0;
		}
	}
}


void operation:: getContoursAndMoment()
{	
	ColorTheresholding();
	cv::cvtColor(detectWindow, detectWindow, CV_BGRA2GRAY);//形态学处理先要化为灰度图
	cv::Mat mask(3, 3, CV_8U, cv::Scalar(1));
	cv::morphologyEx(detectWindow, detectWindow, cv::MORPH_CLOSE, mask);

	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(detectWindow, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
	int cmin = 20;
	int cmax = 120;
	std::vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end())
	{
		std::cout << "size: " << itc->size() << std::endl;
		if (itc->size() < cmin || itc->size() > cmax)
		{
			itc = contours.erase(itc);//除去不符合要求的的轮廓
		}
		else itc++;
	}
	std::cout << "The number of contours is " << contours.size() << std::endl;
	if (contours.size() == 1||contours.size()==2||contours.size()==6)
	{
		std::vector<std::vector<cv::Point>>::const_iterator it = contours.begin();

		int i = 0;
		while (it != contours.end())
		{
			cv::Moments mom = cv::moments(cv::Mat(*it++));
			momentpoints[i] = cv::Point(mom.m10 / mom.m00+detectWindowPosition.x, mom.m01 / mom.m00+ detectWindowPosition.y);
			cv::circle(image, momentpoints[i], 2, cv::Scalar(0, 255, 0), 2);
			i++;
		}
		if (contours.size() == 1)
		{
			momentpoints[1] = momentpoints[0];
		}
		if (contours.size() == 2 || contours.size() == 6)
		{
			if (momentpoints[0].x < momentpoints[1].x)
			{
				cv::Point tempP;
				tempP = momentpoints[0];
				momentpoints[0] = momentpoints[1];
				momentpoints[1] = tempP;
			}//保证脚尖在脚跟右边，和摄像头和跑步机相对位置有关
		}
	}
	else
	{
		std::cout << "Contours numbers are wrong"<<std::endl;
		return;
	}
}