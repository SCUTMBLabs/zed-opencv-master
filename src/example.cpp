// ZED includes
#include <sl_zed/Camera.hpp>

// Sample includes
#include <SaveDepth.hpp>

#include"DataProcess.h"

using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
void matchImage(cv::Mat image1, cv::Mat image2, cv::Point momentPoint[2][6]);


int __main() {

	Camera zed;
	operation Operation;
	DataProcess dataProcess;
	std::ofstream outfile;

	outfile.open("C:\\Users\\Xie\\Desktop\\data.txt");
	cv::namedWindow("RIGHT");
	cv::setMouseCallback("RIGHT", operation::Mouse_getColor, 0);

	InitParameters init_params;
	init_params.camera_resolution = RESOLUTION_HD720;init_params.camera_fps = 60;

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

	Mat image_zed(new_width, new_height, MAT_TYPE_8U_C4);

	bool isTheFirstTimeOfLeft = true;
	bool isTheFirstTimeOfRight = true;

	cv::Point momentpointsEx[2][6];
	cv::Point momentPoint[2][6];

	while (1) 
	{
		cv::Mat image1;cv::Mat image2;
		if (zed.grab() == SUCCESS)//消耗3ms
		{
			int picture = 0;
			while (picture < 2)
			{
				if (picture == 0)
				{
					zed.retrieveImage(image_zed, VIEW_LEFT, MEM_CPU, new_width, new_height);//消耗5ms
					Operation.image = slMat2cvMat(image_zed);
				}
				if (picture == 1)
				{
					zed.retrieveImage(image_zed, VIEW_RIGHT, MEM_CPU, new_width, new_height);//消耗5ms
					Operation.image = slMat2cvMat(image_zed);
					
				}
				dataProcess.image = Operation.image;
	
				if (operation::getColors)
				{
					if (isTheFirstTimeOfLeft||isTheFirstTimeOfRight)
					{
						Operation.detectWindow = Operation.image.clone();
						Operation.detectWindowPosition = cv::Point(0, 0);
						Operation.getContoursAndMoment();
						for (int i = 0; i < 6; i++)
						{
							momentPoint[picture][i] = Operation.momentpoints[i];
							momentpointsEx[picture][i] = Operation.momentpoints[i];
						}
						if (picture == 0)
						{
							isTheFirstTimeOfLeft = false;
						}
						if (picture == 1)
						{
							isTheFirstTimeOfRight = false;
						}
						Operation.threshold = 120;
					}
					else
					{
						for (int i = 0; i < 6; i++)
						{
							int temp_momentpointpositionx = static_cast<int>(momentPoint[picture][i].x+0.85*(momentPoint[picture][i].x - momentpointsEx[picture][i].x) - 20);
							int temp_momentpointpositiony = static_cast<int>(momentPoint[picture][i].y+0.85*(momentPoint[picture][i].y - momentpointsEx[picture][i].y) - 20);//位置状态方程
							Operation.detectWindowPosition = cv::Point(temp_momentpointpositionx, temp_momentpointpositiony);
							Operation.detectWindow = Operation.image(cv::Rect(temp_momentpointpositionx, temp_momentpointpositiony, 40, 40)).clone();
							cv::rectangle(Operation.image, cv::Point(temp_momentpointpositionx, temp_momentpointpositiony), cv::Point(temp_momentpointpositionx + 40, temp_momentpointpositiony + 40), cv::Scalar(0, 255, 0), 2, 8);
							momentpointsEx[picture][i] = momentPoint[picture][i];

							Operation.getContoursAndMoment();
							if (i != 1)
							{
								momentPoint[picture][i] = Operation.momentpoints[0];
							}
							else
							{
								momentPoint[picture][i] = Operation.momentpoints[1];//保证当窗口出现两个轮廓时取点的合理性
							}
						}
					}
					for (int j = 0; j < 6; j++)
					{
						dataProcess.points[picture][j] = momentPoint[picture][j];
					}
					cv::line(Operation.image, momentPoint[picture][0], momentPoint[picture][1], cv::Scalar(0, 255, 0), 2);
					cv::line(Operation.image, momentPoint[picture][2], momentPoint[picture][3], cv::Scalar(0, 255, 0), 2);
					cv::line(Operation.image, momentPoint[picture][4], momentPoint[picture][5], cv::Scalar(0, 255, 0), 2);
				}//框内为取色后的操作
				if (picture == 0)
				{
					cv::imshow("LEFT", Operation.image);
					image1 = Operation.image.clone();//不使用clone（），image1和image2会指向同一个内存
				}
				if (picture == 1)
				{
					cv::imshow("RIGHT", Operation.image);
					image2 = Operation.image.clone();
				}
				picture++;
			}//框内为循环操作左右两张图片
			if (operation::getColors)
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
}
