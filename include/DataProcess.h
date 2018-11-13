#pragma once

#include"operation.h"

class DataProcess
{
	bool gettime = false;
	double second, millisecond, deltat = 0;

	cv::Point2d thigh, shank, foot;
	const double pi = 3.1415926;

public:
	DataProcess();
	~DataProcess();

	void getTime();
	void mapTo3D();
	void getJointAngle();
	cv::Point points[2][6];
	cv::Point3d _3Dpoints[6];
	cv::Mat image;
	double time = 0;
	double hip, knee, ankle;
};

