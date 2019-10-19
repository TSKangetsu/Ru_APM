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
	cv::Mat ins = Base::Matcom::ImageGet("/Data/da.png");
	CameraCOM::MarkOutModule ems;
	cv::Mat inas = ems.ColorCut(ins);
	ems.ImgMarkOut(inas);
	return 0;
}