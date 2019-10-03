#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
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
	I2Ccontrol_REC::PCA9685_AirSet ins;
	//cv::Mat ims = Base::Matcom::ImageGet(".\\Data\\QJ.jpg");
	//cv::Mat IMS;
	//cv::Mat IMSO;
	//cv::cvtColor(ims, IMS, cv::COLOR_BGR2HSV);
	//imshow("ins", IMS);
	//cv::waitKey();
	//cv::inRange(IMS, cv::Scalar(0, 255, 0), cv::Scalar(0,255,0), IMSO);
	//imshow("ins", IMSO);
	//cv::waitKey();
}