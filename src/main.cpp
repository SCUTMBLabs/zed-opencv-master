// ZED includes
#include <sl_zed/Camera.hpp>
#define size_detection_window 30
// Sample includes
#include <SaveDepth.hpp>
#include <cmath>
#include <iostream>
#include"DataProcess.h"
#include "TemplateMatch.h"
#include <vector>
//#include "utils.hpp"
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
	cv::setMouseCallback("RIGHT", templ.Mouse_getColor, 0);

	InitParameters init_params;
	init_params.camera_resolution = RESOLUTION_HD720;//�ֱ���
	init_params.camera_fps = 60;//֡��
	//init_params.depth_mode = DEPTH_MODE_NONE;
	init_params.svo_input_filename.set("F:\\zikang\\zed-recording-video\\video.svo");

	ERROR_CODE err = zed.open(init_params);
	if (err != SUCCESS) 
	{
		printf("%s\n", toString(err).c_str());
		zed.close();
		return 1; 
	}

	// Prepare new image size to retrieve half-resolution images
	Resolution image_size = zed.getResolution();
	int new_width = int(image_size.width / 2);
	int new_height = int(image_size.height / 2);
	std::cout << new_width << " " << new_height << std::endl;
	Mat image_zed_left(new_width, new_height, MAT_TYPE_8U_C3);
	Mat image_zed_right(new_width, new_height, MAT_TYPE_8U_C3); //
	std::cout << image_zed_right.getDataType() << std::endl;
	bool isTheFirstTimeOfLeft = true;
	bool isTheFirstTimeOfRight = true;
	cv::Point ex_point[2][6];  //��һ֡�ĵ�����
	cv::Point current_point[2][6];	//ǰһ֡������
	while (1) 
	{
		cv::Mat image1;cv::Mat image2;
		if (zed.grab() == SUCCESS)//����3ms��ȡͼƬ
		{
			zed.retrieveImage(image_zed_right, VIEW_RIGHT, MEM_CPU, new_width, new_height);//����5ms
			zed.retrieveImage(image_zed_left, VIEW_LEFT, MEM_CPU, new_width, new_height);//����5ms
			templ.image_r = slMat2cvMat(image_zed_right);
			templ.image_l = slMat2cvMat(image_zed_left);  //��ʽת��
			// ɾȥ���һ������Ҫ��ͨ��
			cv::cvtColor(templ.image_l, templ.image_l, cv::COLOR_BGRA2BGR);
			cv::cvtColor(templ.image_r, templ.image_r, cv::COLOR_BGRA2BGR);
			templ.image_r.convertTo(templ.image_r, CV_8U);	
			templ.image_l.convertTo(templ.image_l, CV_8U);
			assert(templ.image_r.channels() == 3&& templ.image_l.channels()==3);
			//dataProcess.image = templ.image;
			if ((!(templ.auto_templ_left&& templ.auto_templ_right))&&templ.getColors)
			{
				for (int picture = 0; picture < 2; picture++)
				{
					if (picture == 0)
					{
						templ.image = templ.image_l;
					}
					if(picture == 1)
					{
						templ.image = templ.image_r;
					}
					templ.auto_Template(picture);
				}
			}
			if (templ.auto_templ_left&& templ.auto_templ_right)
			{
					
				if (isTheFirstTimeOfLeft||isTheFirstTimeOfRight)
				{						 
					//�ǵ�һ�δ򿪣�������ң�����ģ��λ�ó�ʼ��current_point and ex_point
						
					
					for (int i = 0; i < 6; i++)
					{
						templ.image = templ.image_l;
						current_point[0][i] = ex_point[0][i] = templ.init_template(i);
						//printf("left initialize\n");
					}
					isTheFirstTimeOfLeft = false;
					
					
					for (int i = 0; i < 6; i++)
					{
						templ.image = templ.image_r.clone();
						current_point[1][i] = ex_point[1][i] = templ.init_template(i+6);
						//printf("right initialize\n");
					}
					isTheFirstTimeOfRight = false;
						 
				}
				else
				{
					//���á�α�������˲�������Ԥ���λ��
					for (int picture = 0; picture < 2; picture++)
					{
						if (picture == 0)
						{
							templ.image = templ.image_l;//ǳ����ָ��ͬһ�ڴ棬�Ա㻭ͼ
						}
						if (picture == 1)
						{
							templ.image = templ.image_r;
						}
						for (int i = 0; i < 6; i++)
						{
							templ.predict_x[i] = static_cast<int>(0.88 * (current_point[picture][i].x - ex_point[picture][i].x) + current_point[picture][i].x 
																			- 0.1 * (ex_point[picture][i].x - templ.start_point[picture][i].x));
							templ.predict_y[i] = static_cast<int>(0.8 * (current_point[picture][i].y - ex_point[picture][i].y) + current_point[picture][i].y 
																			- 0 * (ex_point[picture][i].y - templ.start_point[picture][i].y));//λ��״̬����	
						}
						double distance = pow(sqrt(templ.predict_x[1] - templ.predict_x[0]) + sqrt(templ.predict_y[1] - templ.predict_y[0]), 2);
						if (size_detection_window*0.2<distance< size_detection_window*0.6)
						{
							templ.predict_x[1] += int(0.3*size_detection_window);
							templ.predict_x[0] -= int(0.3*size_detection_window);
						
						}
						if (distance <= 0.1*size_detection_window)
						{
							// ���ڽӽ��������³�ʼ��
							templ.auto_templ_left = false;
							templ.auto_templ_right = false;
						}
						for (int i = 0; i < 6; i++)
						{
							if (templ.predict_x[i] - size_detection_window / 2 <= 0)
							{
								templ.predict_x[i] = size_detection_window / 2 + 1;
							}
							if (templ.predict_y[i] - size_detection_window / 2 <= 0)
							{
								templ.predict_y[i] = size_detection_window / 2 + 1;
							}
							if (templ.predict_x[i] + size_detection_window / 2 >= templ.image.cols)
							{
								templ.predict_x[i] = templ.image.cols - size_detection_window / 2 - 1;
							}
							if (templ.predict_y[i] + size_detection_window / 2 >= templ.image.rows)
							{
								templ.predict_y[i] = templ.image.rows - size_detection_window / 2 - 1;
							}
							templ.detectWindowPosition = cv::Point(templ.predict_x[i], templ.predict_y[i]);//Ԥ������ĵ�������ͼ���ϵ�λ��
							//if(temp_momentpointpositionx - sizeOfDet/2>0&& temp_momentpointpositiony - sizeOfDet/2>0&& temp_momentpointpositionx + sizeOfDet/2<templ.image.cols&&temp_momentpointpositiony + sizeOfDet/2<templ.image.rows)
							templ.detectWindow = templ.image(cv::Rect(templ.predict_x[i] - size_detection_window / 2, templ.predict_y[i] - size_detection_window / 2,
								size_detection_window, size_detection_window)).clone();
							//��������ģ��/����
							cv::rectangle(templ.image, cv::Point(templ.predict_x[i] - size_detection_window / 2, templ.predict_y[i] - size_detection_window / 2),
											cv::Point(templ.predict_x[i] + size_detection_window / 2, templ.predict_y[i] + size_detection_window / 2), cv::Scalar(0, 255, 0), 2, 8); //Ԥ������
							cv::circle(templ.image, templ.detectWindowPosition, 3, cv::Scalar(0, 255, 0), 3);//����һ��ͼƬ��Ԥ������
							cv::circle(templ.image, current_point[picture][i], 2, cv::Scalar(0, 0, 0), 2);//��һ�ŵ�ƥ���
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
				}
				for (int j = 0; j < 6; j++)
				{
					dataProcess.points[0][j] = current_point[0][j];
					dataProcess.points[1][j] = current_point[1][j];
				}
				//��������ؽ�֮�������
				cv::line(templ.image_l, current_point[0][0], current_point[0][1], cv::Scalar(255, 0, 0), 2);
				cv::line(templ.image_l, current_point[0][2], current_point[0][3], cv::Scalar(255, 0, 0), 2);
				cv::line(templ.image_l, current_point[0][4], current_point[0][5], cv::Scalar(255, 0, 0), 2);
				cv::line(templ.image_r, current_point[1][0], current_point[1][1], cv::Scalar(255, 0, 0), 2);
				cv::line(templ.image_r, current_point[1][2], current_point[1][3], cv::Scalar(255, 0, 0), 2);
				cv::line(templ.image_r, current_point[1][4], current_point[1][5], cv::Scalar(255, 0, 0), 2);				
			}//����Ϊȡɫ��Ĳ���	
			cv::imshow("LEFT", templ.image_l);
			image1 = templ.image_l.clone();//�����ʹ��clone������image1��image2��ָ��ͬһ���ڴ�
			cv::imshow("RIGHT", templ.image_r);
			image2 = templ.image_r.clone();
		}
		if (templ.auto_templ_left&& templ.auto_templ_right)
		{
			dataProcess.getJointAngle();
			outfile << "time: " << dataProcess.time << "  hip:  " << dataProcess.hip << "  knee:  " << dataProcess.knee << "  ankle:  " << dataProcess.ankle << std::endl;
			matchImage(image1, image2, current_point);
		}
		char key = cv::waitKey(1);
		if (key == 'q') 
		{	break;}
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
	if (image.rows != 0 && image.cols != 0)
	{
		cv::imshow("Match", image);
	} //������ʱ�жϣ���������Ƶ�����ˣ�û�ж���֡�ɹ���ʾ

	//������ΪҪ����Ӳ�����ԻỨ�Ѵ���ʱ�䣬ֻӦ�ڱ�Ҫ����ʱʹ��
 	//char name[20];
 	//static int time=0;
 	//sprintf(name, "frames/%d.jpg",time++);
 	//cv::imwrite(name, image);
}
