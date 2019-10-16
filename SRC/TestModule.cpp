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
	cv::Mat IMSO;
	cv::Mat EnSO;
	cv::Mat GAUS;
	cv::Mat Dele;
	//cv::Mat cvt;

	cv::Mat ims = Base::Matcom::ImageGet("/Data/dc.png");
	//cv::cvtColor(ims, cvt, cv::COLOR_BGR2HSV);
	cv::Mat Ele = cv::getStructuringElement(cv::MORPH_RECT,
		cv::Size(8 * 1, 8 * 1), 
		cv::Point(2,2));
	cv::erode(ims, EnSO, Ele);
	cv::inRange(EnSO, cv::Scalar(0, 0, 0), cv::Scalar(0, 255, 0), IMSO);
	cv::erode(IMSO, EnSO, Ele);
	cv::GaussianBlur(EnSO, GAUS, cv::Size(11, 11), 0);
	cv::dilate(GAUS, Dele, Ele);
	//cv::Mat locations;
	//cv::findNonZero(IMSO, locations);
	//for (int i = 0; i < 100; i++)
	//{
	//	cv::Point loca = locations.at<cv::Point>(i);
	//	std::cout << loca.x << std::endl;
	//}
	imshow("ins", GAUS);
	cv::waitKey();
}