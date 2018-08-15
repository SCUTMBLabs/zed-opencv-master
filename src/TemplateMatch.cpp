#include "TemplateMatch.h"

 


cv::Point TemplateMatch::init_template(int i)
{
	try
	{
		ImageResult = cv::Mat::zeros(Template_batch[i].rows, Template_batch[i].cols, CV_32FC1);
		assert(Template_batch[i].channels() == image.channels());
		cv::matchTemplate(image, Template_batch[i], ImageResult, cv::TM_CCOEFF_NORMED);  //ƥ�䣬���ض��ƥ����
		std::cout << "Imageresult.channels(): "<<ImageResult.channels() << std::endl;
		cv::minMaxLoc(ImageResult, &minValue, &maxValude, &minPoint, &maxPoint, cv::Mat());  //ƥ��������ֵ���������ȡ
		/*char name[20];

		sprintf(name, "%dtenplate.jpg", i);
		cv::imwrite(name, Template_batch[i]);		*/
		//cv::rectangle(templ.image, templ.minPoint, cv::Point(templ.minPoint.x + templ.Template_batch[i].cols, templ.minPoint.y + templ.Template_batch[i].rows), cv::Scalar(0, 0, 255), 2);//��ͼ�л�������λ��				
		Template_batch[i] = image(cv::Rect(maxPoint, cv::Point(maxPoint.x + Template_batch[i].cols,
						maxPoint.y + Template_batch[i].rows)));//���µ�ǰģ��ƥ���ģ��		

	}
	catch (const std::exception& )
	{
		printf("error\n");
	}
	return maxPoint + cv::Point(10, 10); //batch������ͼ���е��е�λ������
	
}

cv::Point TemplateMatch::update_template(int i)
{
	assert(Template_batch[i].channels() == image.channels());
	cv::matchTemplate(detectWindow, Template_batch[i], ImageResult, cv::TM_CCOEFF_NORMED);  //ƥ�䣬���ض��ƥ����
	cv::minMaxLoc(ImageResult, &minValue, &maxValude, &minPoint, &maxPoint, cv::Mat());  //ƥ��������ֵ���������ȡ
	
	 //cv::rectangle(templ.image, templ.minPoint, cv::Point(templ.minPoint.x + templ.Template_batch[i].cols, templ.minPoint.y + templ.Template_batch[i].rows), cv::Scalar(0, 0, 255), 2);//��ͼ�л�������λ��				
	Template_batch[i] = image(cv::Rect(maxPoint, cv::Point(maxPoint.x + Template_batch[i].cols,
						maxPoint.y + Template_batch[i].rows)));//���µ�ǰģ��ƥ���ģ��

	return maxPoint+cv::Point(10,10);//batch�ڼ�ⴰ���е��е�λ������
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
			imageCopy = image.clone();
			Template_batch[index_template] = image(cv::Rect(originalPoint - cv::Point(10, 10), originalPoint + cv::Point(10, 10))); //��ͼ��
			assert(Template_batch[index_template].channels() == image.channels());
			cv::rectangle(imageCopy, originalPoint - cv::Point(10, 10), originalPoint + cv::Point(10, 10), cv::Scalar(255, 0, 0), 2);
			imshow("Man", imageCopy);
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
			  imageCopy = image.clone();
			  Template_batch[index_template] = image(cv::Rect(originalPoint - cv::Point(10, 10), originalPoint + cv::Point(10, 10))); //��ͼ��
			  assert(Template_batch[index_template].channels() == image.channels());
			  cv::rectangle(imageCopy, originalPoint - cv::Point(10, 10), originalPoint + cv::Point(10,10), cv::Scalar(255, 0, 0), 2);
			  imshow("Man", image);
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

int TemplateMatch::index_template = 0;
cv::Mat TemplateMatch::image = cv::Mat::zeros(4, 5, CV_32FC3);
cv::Mat TemplateMatch::imageCopy = cv::Mat::zeros(4, 5, CV_32FC3);