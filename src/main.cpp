// ZED includes
#include <sl_zed/Camera.hpp>
#define size_detection_window 40
// Sample includes
#include <SaveDepth.hpp>
#include <iostream>
#include"DataProcess.h"
#include "TemplateMatch.h"
#include <vector>
using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
void matchImage(cv::Mat image1, cv::Mat image2, cv::Point current_point[2][6]);


int main() {

	Camera zed;
	operation Operation;
	TemplateMatch templ;
	 
	DataProcess dataProcess;
	std::ofstream outfile;

	outfile.open("C:\\Users\\Xie\\Desktop\\data.txt");
	cv::namedWindow("LEFT");
	cv::namedWindow("RIGHT");
	 //Ҫ����windows����ʹ��setMousecallBack
	//cv::setMouseCallback("LEFT", TemplateMatch::on_Mouse_LEFT, 0);
	//cv::setMouseCallback("RIGHT", TemplateMatch::on_Mouse_RIGHT, 0);   //MouseCallBack Ҫ��̬���� 
	cv::setMouseCallback("LEFT", templ.Mouse_getColor, 0);

	InitParameters init_params;
	init_params.camera_resolution = RESOLUTION_HD720;//�ֱ���
	init_params.camera_fps = 60;//֡��

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

	Mat image_zed(new_width, new_height, MAT_TYPE_8U_C3); //
	std::cout << image_zed.getDataType() << std::endl;
	bool isTheFirstTimeOfLeft = true;
	bool isTheFirstTimeOfRight = true;
	cv::Point ex_point[2][6];  //��һ֡�ĵ�����
	cv::Point current_point[2][6];	//ǰһ֡������
	while (1) 
	{
		cv::Mat image1;cv::Mat image2;
		if (zed.grab() == SUCCESS)//����3ms��ȡͼƬ
		{
			int picture = 0;
			while (picture < 2)
			{
				if (picture == 0)
				{
					zed.retrieveImage(image_zed, VIEW_LEFT, MEM_CPU, new_width, new_height);//����5ms
					templ.image = slMat2cvMat(image_zed);  //��ʽת��
					// ɾȥ���һ������Ҫ��ͨ��
					cv::cvtColor(templ.image, templ.image, cv::COLOR_BGRA2BGR);
					templ.image.convertTo(templ.image, CV_8U);
					templ.image_l = templ.image;

				}
				if (picture == 1)
				{
					zed.retrieveImage(image_zed, VIEW_RIGHT, MEM_CPU, new_width, new_height);//����5ms
					templ.image = slMat2cvMat(image_zed);
					cv::cvtColor(templ.image, templ.image, cv::COLOR_BGRA2BGR);
					templ.image.convertTo(templ.image, CV_8U);
					templ.image_r = templ.image;
				}
				
				std::cout << templ.image.channels() << std::endl;
				assert(templ.image.channels() == 3);
				dataProcess.image = templ.image;
				if ((!(templ.auto_templ_left&& templ.auto_templ_right))&&templ.getColors)
				{
					templ.auto_Template();
					switch (int(templ.auto_templ_left)+int(templ.auto_templ_right))
					{
					case 1:std::cout << "using color to create template for the first time" << std::endl; break;
					case 2:std::cout << "using color to create template for the second time" << std::endl;
						
					}
					
				}
				if (templ.auto_templ_left&& templ.auto_templ_right)
				{
					
					if (isTheFirstTimeOfLeft||isTheFirstTimeOfRight)
					{						 
						//�ǵ�һ�δ򿪣�������ң�����ģ��λ�ó�ʼ��current_point and ex_point
						
						if (picture == 0)
						{
							for (int i = 0; i < 6; i++)
							{
								current_point[picture][i] = ex_point[picture][i] = templ.init_template(i);
								//printf("left initialize\n");
							}
							isTheFirstTimeOfLeft = false;
						}
						if (picture == 1)

						{
							for (int i = 0; i < 6; i++)
							{
								current_point[picture][i] = ex_point[picture][i] = templ.init_template(i+6);
								//printf("right initialize\n");
							}
							isTheFirstTimeOfRight = false;
						}
						 
					}
					else
					{
						//���á�α�������˲�������Ԥ���λ��
						for (int i = 0; i < 6; i++)
						{
							
							int predict_x = static_cast<int>(1.8 * (current_point[picture][i].x -   ex_point[picture][i].x) + templ.start_point[picture][i].x)+ 1*(ex_point[picture][i].x-templ.start_point[picture][i].x);
							int predict_y = static_cast<int>(1.8 * (current_point[picture][i].y -   ex_point[picture][i].y)+ templ.start_point[picture][i].y)+ 1*(ex_point[picture][i].y- templ.start_point[picture][i].y);//λ��״̬����
							templ.detectWindowPosition = cv::Point(predict_x, predict_y);//Ԥ������ĵ�������ͼ���ϵ�λ��
							assert(predict_x - size_detection_window / 2 > 0 && predict_y - size_detection_window / 2 > 0 && predict_x + size_detection_window / 2 < templ.image.cols&&predict_y + size_detection_window / 2 < templ.image.rows);
							//if(temp_momentpointpositionx - sizeOfDet/2>0&& temp_momentpointpositiony - sizeOfDet/2>0&& temp_momentpointpositionx + sizeOfDet/2<templ.image.cols&&temp_momentpointpositiony + sizeOfDet/2<templ.image.rows)
							templ.detectWindow = templ.image(cv::Rect(predict_x - size_detection_window / 2, predict_y - size_detection_window / 2,
								size_detection_window, size_detection_window)).clone();
							//��������ģ��/����
							cv::rectangle(templ.image, cv::Point(predict_x - size_detection_window / 2, predict_y - size_detection_window / 2), cv::Point(predict_x + size_detection_window / 2, predict_y + size_detection_window / 2), cv::Scalar(0, 255, 0), 2, 8); //Ԥ������
							templ.start_point[picture][i] = ex_point[picture][i];
							ex_point[picture][i] = current_point[picture][i];
							// ע�������ĸ���˳��
							if (picture == 0)//������ͼ�ĸ���
							{
								current_point[picture][i] = templ.update_template(i) + templ.detectWindowPosition - cv::Point(size_detection_window / 2, size_detection_window / 2);
							}
							if (picture == 1)//������ͼ�ĸ���
							{
								current_point[picture][i] = templ.update_template(i + 6) + templ.detectWindowPosition - cv::Point(size_detection_window / 2, size_detection_window / 2);//ƥ������
							}
						}

					}
					for (int j = 0; j < 6; j++)
					{
						dataProcess.points[picture][j] = current_point[picture][j];
					}
					//��������ؽ�֮�������
					cv::line(templ.image, current_point[picture][0], current_point[picture][1], cv::Scalar(255, 0, 0), 2);
					cv::line(templ.image, current_point[picture][2], current_point[picture][3], cv::Scalar(255, 0, 0), 2);
					cv::line(templ.image, current_point[picture][4], current_point[picture][5], cv::Scalar(255, 0, 0), 2);
				}//����Ϊȡɫ��Ĳ���
				if (picture == 0)
				{

					assert(templ.image.channels() == 3);
					cv::imshow("LEFT", templ.image);
					image1 = templ.image.clone();//��ʹ��clone������image1��image2��ָ��ͬһ���ڴ�
					//templ.image_l = templ.image.clone();
				}
				if (picture == 1)
				{
					assert(templ.image.channels() == 3);
					cv::imshow("RIGHT", templ.image);
					image2 = templ.image.clone();
					//templ.image_r = templ.image.clone();
				}
				picture++;
			}//����Ϊѭ��������������ͼƬ
			if (templ.auto_templ_left&& templ.auto_templ_right&&(!(isTheFirstTimeOfLeft||isTheFirstTimeOfRight)))
			{
				dataProcess.getJointAngle();
				outfile << "time: " << dataProcess.time << "  hip:  " << dataProcess.hip << "  knee:  " << dataProcess.knee << "  ankle:  " << dataProcess.ankle << std::endl;
				matchImage(image1, image2, current_point);
			}
			
			char key = cv::waitKey(1);
			if (key == 'q') 
			{	break;}
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


void matchImage(cv::Mat image1, cv::Mat image2, cv::Point current_point[2][6])
{
	int rows = image1.rows;
	int cols = image1.cols;
	cv::Mat image(rows, 2 * cols, CV_8UC3);
	for (int i = 0; i < rows; i++)
	{
		uchar*data = image.ptr<uchar>(i);
		uchar*data1 = image1.ptr<uchar>(i);
		uchar*data2 = image2.ptr<uchar>(i);

		for (int j = 0; j < cols; j++)
		{
			data[3 * j] = data1[3 * j]; // ����Ϊʲô����3����������ͨ�����йأ�ע�����ͨ�����޸ģ�����matchImage��������
			data[3 * j + 1] = data1[3 * j + 1];
			data[3 * j + 2] = data1[3 * j + 2];
			//data[3 * j + 3] = data1[3 * j + 3];
		}
		for (int j = cols; j < 2 * cols; j++)
		{
			data[3 * j] = data2[3 * (j-cols)];
			data[3 * j + 1] = data2[3 * (j - cols) + 1];
			data[3 * j + 2] = data2[3 * (j - cols) + 2];
			//data[3 * j + 3] = data2[3 * (j - cols) + 3];
		}
	}
	for (int k = 0; k < 6; k++)
	{
		cv::Point tempPoint[6];
		tempPoint[k].x = current_point[1][k].x + cols;
		tempPoint[k].y = current_point[1][k].y;
		cv::line(image, current_point[0][k], tempPoint[k], cv::Scalar(0, 255, 0), 1);//ע��current_point��ȫ�ֱ���//ʦ����ôд����֪����
	}
	cv::imshow("Match", image);
 	char name[20];
 	static int time=0;
	 
 	sprintf(name, "%d.jpg",time++);
 	cv::imwrite(name, image);
}
