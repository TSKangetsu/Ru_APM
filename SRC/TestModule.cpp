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
#include <time.h>
#endif

#ifdef windows
#include <Windows.h>
#endif

int main(int argc, char* argv[])
{
	CameraCOM::MarkOutModule Detect(0);
	cv::Mat deal = Base::Matcom::ImageGet("/Data/dc.png");
	deal = Detect.ColorCut(deal, cv::Scalar(12, 12));
	cv::imshow("", deal);
	cv::waitKey();
}