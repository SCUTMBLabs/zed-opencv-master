#include "TemplateMatch.h"

#define size_template 10


cv::Point TemplateMatch::init_template(int i)
{
	try
	{	
		ImageResult = cv::Mat::zeros(Template_batch[i].rows, Template_batch[i].cols, CV_8UC1);
		assert(Template_batch[i].channels() == image.channels());
		cv::matchTemplate(image, Template_batch[i], ImageResult,  CV_TM_SQDIFF);  //匹配，返回多个匹配结果
		std::cout << "Imageresult.channels(): "<<ImageResult.channels() << std::endl;
		cv::minMaxLoc(ImageResult, &minValue, &maxValude, &minPoint, &maxPoint, cv::Mat());  //匹配结果最优值及其坐标获取
		//cv::rectangle(templ.image, templ.minPoint, cv::Point(templ.minPoint.x + templ.Template_batch[i].cols, templ.minPoint.y + templ.Template_batch[i].rows), cv::Scalar(0, 0, 255), 2);//在图中画出最有位置				
		//Template_batch[i] = image(cv::Rect(maxPoint, cv::Point(maxPoint.x + Template_batch[i].cols,
			//			maxPoint.y + Template_batch[i].rows)));//更新当前模板匹配的模板		

	}
	catch (const std::exception& )
	{
		printf("error\n");
	}
	return minPoint + cv::Point(size_template / 2, size_template / 2); //batch在整个图像中的中点位置坐标
	
}

cv::Point TemplateMatch::update_template(int i) 
{


	//注意detectwindow 和template_batch 的通道数是否相同！！！！
	cv::matchTemplate(detectWindow, Template_batch[i], ImageResult, CV_TM_SQDIFF);  //匹配，返回多个匹配结果
	cv::minMaxLoc(ImageResult, &minValue, &maxValude, &minPoint, &maxPoint, cv::Mat());  //匹配结果最优值及其坐标获取
	
	//cv::rectangle(templ.image, templ.minPoint, cv::Point(templ.minPoint.x + templ.Template_batch[i].cols, templ.minPoint.y + templ.Template_batch[i].rows), cv::Scalar(0, 0, 255), 2);//在图中画出最有位置				
	// Template_batch[i] = image(cv::Rect(maxPoint, cv::Point(maxPoint.x + Template_batch[i].cols,
						//maxPoint.y + Template_batch[i].rows)));//更新当前模板匹配的模板 

	return minPoint +cv::Point(size_template / 2, size_template / 2);//batch在检测窗口中的中点位置坐标
}



  void  TemplateMatch::on_Mouse_RIGHT(int event, int x, int y, int, void*)
{

	if( 6 <= index_template < 12)
	{
		 
		if (event == CV_EVENT_LBUTTONDOWN)
		{
			leftButtonDownFlag = true; //标志位
			originalPoint = cv::Point(x, y);  //设置左键按下点的矩形起点
			processPoint = originalPoint;
		}
		if (event == CV_EVENT_LBUTTONUP&&leftButtonDownFlag)
		{
			leftButtonDownFlag = false;
			imageCopy = image_r.clone();
			Template_batch[index_template] = image_r(cv::Rect(originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2))); //子图像
			assert(Template_batch[index_template].channels() == image.channels());
			cv::rectangle(imageCopy, originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2), cv::Scalar(255, 0, 0), 2);
			imshow("right_template", imageCopy);
			imshow("right_template_batch", Template_batch[index_template]);
			start_point[1][index_template-6] = originalPoint;//注意index_template范围在6-11，而start_point的第二个维度只有6个值
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
			  leftButtonDownFlag = true; //标志位
			  originalPoint = cv::Point(x, y);  //设置左键按下点的矩形起点
			  processPoint = originalPoint;
		  }
		  if (event == CV_EVENT_LBUTTONUP&&leftButtonDownFlag)
		  {
			  leftButtonDownFlag = false;
			  imageCopy = image_l.clone();
			  Template_batch[index_template] = image_l(cv::Rect(originalPoint - cv::Point(size_template / 2, size_template / 2), originalPoint + cv::Point(size_template / 2, size_template / 2))); //子图像
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

cv::Mat TemplateMatch::image_l = cv::Mat::zeros(4, 5, CV_8UC3);//用来初始化左边鼠标的模板
cv::Mat TemplateMatch::image_r = cv::Mat::zeros(4, 5, CV_8UC3);
int TemplateMatch::index_template = 0;
cv::Mat TemplateMatch::image = cv::Mat::zeros(4, 5, CV_8UC3);
cv::Mat TemplateMatch::imageCopy = cv::Mat::zeros(4, 5, CV_8UC3);
