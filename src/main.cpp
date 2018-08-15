// ZED includes
#include <sl_zed/Camera.hpp>

// Sample includes
#include <SaveDepth.hpp>
#include <iostream>
#include"DataProcess.h"
#include "TemplateMatch.h"
#include <vector>
using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
void matchImage(cv::Mat image1, cv::Mat image2, cv::Point momentPoint[2][6]);


int main() {

	Camera zed;
	operation Operation;
	TemplateMatch templ;
	DataProcess dataProcess;
	std::ofstream outfile;

	outfile.open("C:\\Users\\Xie\\Desktop\\data.txt");
	cv::namedWindow("RIGHT");
	cv::namedWindow("LEFT");  //要定义windows才能使用setMousecallBack
	cv::setMouseCallback("RIGHT", TemplateMatch::on_Mouse_RIGHT, 0);   //MouseCallBack 要求静态函数 
	cv::setMouseCallback("LEFT", TemplateMatch::on_Mouse_LEFT, 0);

	InitParameters init_params;
	init_params.camera_resolution = RESOLUTION_HD720;//分辨率
	init_params.camera_fps = 60;//帧数

	ERROR_CODE err = zed.open(init_params);
	if (err != SUCCESS) 
	{
		printf("%s\n", toString(err).c_str());
		zed.close();
		return 1; 
	}

	// Prepare new image size to retrieve half-resolution images
	Resolution image_size = zed.getResolution();
	int new_width = image_size.width / 2;
	int new_height = image_size.height / 2;
	std::cout << new_width << " " << new_height << std::endl;

	Mat image_zed(new_width, new_height, MAT_TYPE_8U_C3); //这里的通道数为4，最后一个通道必须除去不用

	bool isTheFirstTimeOfLeft = true;
	bool isTheFirstTimeOfRight = true;

	cv::Point momentpointsEx[2][6];  //上一帧的点坐标
	cv::Point momentPoint[2][6];	//前一帧点坐标
	while (1) 
	{
		cv::Mat image1;cv::Mat image2;
		if (zed.grab() == SUCCESS)//消耗3ms，取图片
		{
			int picture = 0;
			while (picture < 2)
			{
				if (picture == 0)
				{
					zed.retrieveImage(image_zed, VIEW_LEFT, MEM_CPU, new_width, new_height);//消耗5ms
					templ.image = slMat2cvMat(image_zed);  //格式转换
				}
				if (picture == 1)
				{
					zed.retrieveImage(image_zed, VIEW_RIGHT, MEM_CPU, new_width, new_height);//消耗5ms
					templ.image = slMat2cvMat(image_zed);
					
				}
				dataProcess.image = templ.image;
				// 删去最后一个不需要的通道
				/*std::vector<cv::Mat> channels;
				cv::split(templ.image, channels);
				channels.pop_back();
				cv::merge(channels, templ.image);
				templ.image.convertTo(templ.image, CV_8U);*/
				if (TemplateMatch::gettempl_RIGHT&& TemplateMatch::gettempl_LEFT)
				{
					
					if (isTheFirstTimeOfLeft||isTheFirstTimeOfRight)
					{
						 
						 
						//是第一次打开（左或者右）则用模板位置初始化momentPoint
						
						if (picture == 0)
						{
							for (int i = 0; i < 6; i++)
							{
								momentPoint[picture][i] = momentpointsEx[picture][i] = templ.init_template(i);
								printf("left\n");
							}
							isTheFirstTimeOfLeft = false;
						}
						if (picture == 1)

						{
							for (int i = 0; i < 6; i++)
							{
								momentPoint[picture][i] = momentpointsEx[picture][i] = templ.init_template(i+6);
								printf("right\n");
							}
							isTheFirstTimeOfRight = false;
						}
						 
					}
					else
					{
						//利用“伪卡尔曼滤波”更新预测的位置
						for (int i = 0; i < 6; i++)
						{
							int temp_momentpointpositionx = static_cast<int>(momentPoint[picture][i].x+0.85*(momentPoint[picture][i].x - momentpointsEx[picture][i].x));
							int temp_momentpointpositiony = static_cast<int>(momentPoint[picture][i].y+0.85*(momentPoint[picture][i].y - momentpointsEx[picture][i].y));//位置状态方程
							templ.detectWindowPosition = cv::Point(temp_momentpointpositionx, temp_momentpointpositiony);//预测框中心点在整个图像上的位置
							assert(temp_momentpointpositionx - 30 > 0 && temp_momentpointpositiony - 30 > 0 && temp_momentpointpositionx + 30 < templ.image.cols&&temp_momentpointpositiony + 30 < templ.image.rows);
							//if(temp_momentpointpositionx - 30>0&& temp_momentpointpositiony - 30>0&& temp_momentpointpositionx + 30<templ.image.cols&&temp_momentpointpositiony + 30<templ.image.rows)
							templ.detectWindow = templ.image(cv::Rect(temp_momentpointpositionx-30, temp_momentpointpositiony-30, 
																60, 60)).clone();
							//templ.detectWindow = templ.image;
							//画出六个模板/矩形
							cv::rectangle(templ.image, cv::Point(temp_momentpointpositionx-30, temp_momentpointpositiony-30), cv::Point(temp_momentpointpositionx + 30, temp_momentpointpositiony + 30), cv::Scalar(0, 255, 0), 2, 8); //预测中心
							momentpointsEx[picture][i] = momentPoint[picture][i];	
							if(picture==0)
								momentPoint[picture][i]=templ.update_template(i)+ templ.detectWindowPosition-cv::Point(30,30);
							if(picture==1)
								momentPoint[picture][i] = templ.update_template(i+6)+ templ.detectWindowPosition - cv::Point(30, 30);//匹配中心

						}

					}
					for (int j = 0; j < 6; j++)
					{
						dataProcess.points[picture][j] = momentPoint[picture][j];
					}
					//画出三组关节之间的连线
					cv::line(templ.image, momentPoint[picture][0], momentPoint[picture][1], cv::Scalar(0, 255, 0), 2);
					cv::line(templ.image, momentPoint[picture][2], momentPoint[picture][3], cv::Scalar(0, 255, 0), 2);
					cv::line(templ.image, momentPoint[picture][4], momentPoint[picture][5], cv::Scalar(0, 255, 0), 2);
				}//框内为取色后的操作
				if (picture == 0)
				{
					cv::imshow("LEFT", templ.image);
					image1 = templ.image.clone();//不使用clone（），image1和image2会指向同一个内存
				}
				if (picture == 1)
				{
					cv::imshow("RIGHT", templ.image);
					image2 = templ.image.clone();
				}
				picture++;
			}//框内为循环操作左右两张图片
			if (TemplateMatch::gettempl_RIGHT && TemplateMatch::gettempl_LEFT)
			{
				dataProcess.getJointAngle();
				outfile << "time:   " << dataProcess.time << "   hip:   " << dataProcess.hip << "   knee:   " << dataProcess.knee << "   ankle:   " << dataProcess.ankle << std::endl;
				matchImage(image1, image2, momentPoint);
			}
			
			char key = cv::waitKey(1);
			if (key == 'q') 
			{
				break;
			}
		}
	}
	zed.close();
	return 0;
}


cv::Mat slMat2cvMat(Mat& input) {
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
	case MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
	case MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
	case MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
	case MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
	case MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
	case MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
	case MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
	case MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
	default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	// cv::Mat and sl::Mat will share a single memory structure
	return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}


void matchImage(cv::Mat image1, cv::Mat image2, cv::Point momentPoint[2][6])
{
	int rows = image1.rows;
	int cols = image1.cols;
	cv::Mat image(rows, 2 * cols, CV_8UC4);
	for (int i = 0; i < rows; i++)
	{
		uchar*data = image.ptr<uchar>(i);
		uchar*data1 = image1.ptr<uchar>(i);
		uchar*data2 = image2.ptr<uchar>(i);

		for (int j = 0; j < cols; j++)
		{
			data[4 * j] = data1[4 * j];
			data[4 * j + 1] = data1[4 * j + 1];
			data[4 * j + 2] = data1[4 * j + 2];
			data[4 * j + 3] = data1[4 * j + 3];
		}
		for (int j = cols; j < 2 * cols; j++)
		{
			data[4 * j] = data2[4 * (j-cols)];
			data[4 * j + 1] = data2[4 * (j - cols) + 1];
			data[4 * j + 2] = data2[4 * (j - cols) + 2];
			data[4 * j + 3] = data2[4 * (j - cols) + 3];
		}
	}
	for (int k = 0; k < 6; k++)
	{
		cv::Point tempPoint[6];
		tempPoint[k].x = momentPoint[1][k].x + cols;
		tempPoint[k].y = momentPoint[1][k].y;
		cv::line(image, momentPoint[0][k], tempPoint[k], cv::Scalar(0, 255, 0), 1);//注意momentPoint是全局变量
	}
	cv::imshow("Match", image);
	char name[20];
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf(name, "%d.jpg", sys.wSecond+ sys.wMilliseconds);
	cv::imwrite(name, image);
}
