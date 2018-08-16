#include "TemplateMatch.h"

#define size_template 10


cv::Point TemplateMatch::init_template(int i)
{
	try
	{	
		ImageResult = cv::Mat::zeros(Template_batch[i].rows, Template_batch[i].cols, CV_8UC1);
		assert(Template_batch[i].channels() == image.channels());
		cv::matchTemplate(image, Template_batch[i], ImageResult,  CV_TM_SQDIFF);  //ƥ�䣬���ض��ƥ����
		std::cout << "Imageresult.channels(): "<<ImageResult.channels() << std::endl;
		cv::minMaxLoc(ImageResult, &minValue, &maxValude, &minPoint, &maxPoint, cv::Mat());  //ƥ��������ֵ���������ȡ
		//cv::rectangle(templ.image, templ.minPoint, cv::Point(templ.minPoint.x + templ.Template_batch[i].cols, templ.minPoint.y + templ.Template_batch[i].rows), cv::Scalar(0, 0, 255), 2);//��ͼ�л�������λ��				
		//Template_batch[i] = image(cv::Rect(maxPoint, cv::Point(maxPoint.x + Template_batch[i].cols,
			//			maxPoint.y + Template_batch[i].rows)));//���µ�ǰģ��ƥ���ģ��		

	}
	catch (const std::exception& )
	{
		printf("error\n");
	}
	return minPoint + cv::Point(size_template / 2, size_template / 2); //batch������ͼ���е��е�λ������
	
}

cv::Point TemplateMatch::update_template(int i) 
{


	//ע��detectwindow ��template_batch ��ͨ�����Ƿ���ͬ��������
	cv::matchTemplate(detectWindow, Template_batch[i], ImageResult, CV_TM_SQDIFF);  //ƥ�䣬���ض��ƥ����
	cv::minMaxLoc(ImageResult, &minValue, &maxValude, &minPoint, &maxPoint, cv::Mat());  //ƥ��������ֵ���������ȡ
	
	//cv::rectangle(templ.image, templ.minPoint, cv::Point(templ.minPoint.x + templ.Template_batch[i].cols, templ.minPoint.y + templ.Template_batch[i].rows), cv::Scalar(0, 0, 255), 2);//��ͼ�л�������λ��				
	// Template_batch[i] = image(cv::Rect(maxPoint, cv::Point(maxPoint.x + Template_batch[i].cols,
						//maxPoint.y + Template_batch[i].rows)));//���µ�ǰģ��ƥ���ģ�� 

	return minPoint +cv::Point(size_template / 2, size_template / 2);//batch�ڼ�ⴰ���е��е�λ������
}



  void  TemplateMatch::on_Mouse_RIGHT(int event, int x, int y, int, void*)
{

	if( 6 <= index_template < 12)
	{
		 
		if (event == CV_EVENT_LBUTTONDOWN)
		{
			leftButtonDownFlag = true; //��־λ
			originalPoint = cv::Point(x, y);  //����������µ�ľ������
			processPoint = originalPoint;
		}
		if (event == CV_EVENT_LBUTTONUP&&leftButtonDownFlag)
		{
			leftButtonDownFlag = false;
			imageCopy = image_r.clone();
			Template_batch[index_template] = image_r(cv::Rect(originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2))); //��ͼ��
			assert(Template_batch[index_template].channels() == image.channels());
			cv::rectangle(imageCopy, originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2), cv::Scalar(255, 0, 0), 2);
			imshow("right_template", imageCopy);
			imshow("right_template_batch", Template_batch[index_template]);
			start_point[1][index_template-6] = originalPoint;//ע��index_template��Χ��6-11����start_point�ĵڶ���ά��ֻ��6��ֵ
			cv::waitKey(100);
			index_template++;
		}
		
	}
	if (index_template == 12)
		gettempl_RIGHT = true;
		 
		
}
  void  TemplateMatch::on_Mouse_LEFT(int event, int x, int y, int, void*)
  {

	  if (index_template < 6)
	  {

		  if (event == CV_EVENT_LBUTTONDOWN)
		  {
			  leftButtonDownFlag = true; //��־λ
			  originalPoint = cv::Point(x, y);  //����������µ�ľ������
			  processPoint = originalPoint;
		  }
		  if (event == CV_EVENT_LBUTTONUP&&leftButtonDownFlag)
		  {
			  leftButtonDownFlag = false;
			  imageCopy = image_l.clone();
			  Template_batch[index_template] = image_l(cv::Rect(originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2))); //��ͼ��
			  assert(Template_batch[index_template].channels() == image.channels());
			  cv::rectangle(imageCopy, originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2), cv::Scalar(255, 0, 0), 2);
			  imshow("left_template", imageCopy);
			  imshow("left_template_batch", Template_batch[index_template]);
			  start_point[0][index_template] = originalPoint;
			  cv::waitKey(100);
			  index_template++;
			 
		  }

	  }
	 if(index_template == 6)
		  gettempl_LEFT = true;
	   

  }
std::vector<cv::Mat> TemplateMatch::Template_batch(12);
bool TemplateMatch::gettempl_RIGHT = false;
bool TemplateMatch::gettempl_LEFT = false;
bool TemplateMatch::leftButtonDownFlag = false;
cv::Point TemplateMatch::originalPoint = cv::Point(0, 0);
cv::Point TemplateMatch::processPoint = cv::Point(0, 0);
cv::Point TemplateMatch::start_point[2][6] = { {cv::Point(0, 0),cv::Point(0, 0),cv::Point(0, 0) ,cv::Point(0, 0) ,cv::Point(0, 0) ,cv::Point(0, 0) },
											{cv::Point(0, 0),cv::Point(0, 0),cv::Point(0, 0) ,cv::Point(0, 0) ,cv::Point(0, 0) ,cv::Point(0, 0) } };

cv::Mat TemplateMatch::image_l = cv::Mat::zeros(4, 5, CV_8UC3);//������ʼ���������ģ��
cv::Mat TemplateMatch::image_r = cv::Mat::zeros(4, 5, CV_8UC3);
int TemplateMatch::index_template = 0;
cv::Mat TemplateMatch::image = cv::Mat::zeros(4, 5, CV_8UC3);
cv::Mat TemplateMatch::imageCopy = cv::Mat::zeros(4, 5, CV_8UC3);
