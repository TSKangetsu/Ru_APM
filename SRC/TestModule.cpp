#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/LinuxClient.hpp"
#include "RaspberrySensors/M_Sensors.hpp"
#include "ConfigSetter/M_Config.hpp"
#ifdef linux
#include <unistd.h>
#endif

#ifdef windows
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
	//CameraCOM::FramePost ins;
	//ins.CameraCheck(0);
	//cv::Mat ins = Base::Matcom::ImageGet("C:\\Users\\x4030\\OneDrive\\Documents\\Project Code TSKangetsu\\CPP.Ru\\Ru_APM\\Data\\QJ.jpg");
	cv::Mat ins = cv::imread("C:\\Users\\x4030\\OneDrive\\Documents\\Project Code TSKangetsu\\CPP.Ru\\Ru_APM\\Data\\QJ.jpg", cv::IMREAD_GRAYSCALE);
	cv::imshow("insa", ins);
	cv::waitKey();
	CameraCOM::MatDeBlur insa;
	cv::Mat ia = insa.FrameOFFDEBlur(ins);
	imshow("uins", ia);
	cv::waitKey();
}
