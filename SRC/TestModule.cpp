#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <iostream>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/ComClient.hpp"
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
	cv::Mat origin = Base::Matcom::ImageGet("da.png");
	CameraCOM::MarkOutModule tas;
	cv::Mat deal = tas.ColorCut(origin);
	std::cout <<"found:" << tas.ImgMarkOut(deal) << "\n";
	cv::imshow("test", deal);
	cv::waitKey();
	return 0;
}