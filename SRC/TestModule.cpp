#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <iostream>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/LinuxClient.hpp"
#include "ConfigSetter/M_Config.hpp"
#include "AirControl/RECConnect/M_RECsetter.hpp"
#ifdef linux
#include <unistd.h>
#endif

#ifdef windows
#include <Windows.h>
#endif

int main(int argc, char* argv[])
{
	cv::Mat ims = Base::Matcom::ImageGet("/Data/dc.png");
	cv::Mat IMSHVS;
	cv::cvtColor(ims , IMSHVS , cv::COLOR_BGR2HSV);
	imshow("ins", IMSHVS);	cv::waitKey();
	//
	cv::Mat IMSO;
	cv::inRange(IMSHVS, cv::Scalar(0, 0, 0), cv::Scalar(0, 255, 128), IMSO);
	cv::Mat locations;
	cv::findNonZero(IMSO, locations);
	for (int i = 0; i < 100; i++)
	{
		cv::Point loca = locations.at<cv::Point>(i);
		std::cout << loca.x << std::endl;
	}
	imshow("ins", IMSO);
	cv::waitKey();
}