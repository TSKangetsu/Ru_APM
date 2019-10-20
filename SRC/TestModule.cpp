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
	CameraCOM::FramePost AsyncTest(5);
	int i;
	std::cin >> i;
	return 0;
}