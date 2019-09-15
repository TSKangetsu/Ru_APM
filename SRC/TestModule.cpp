#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/LinuxClient.hpp"
#include "RaspberrySensors/M_Sensors.hpp"
#include "ConfigSetter/M_Config.hpp"

int main(int argc, char *argv[])
{
	int argvs;
	while ((argvs = getopt(argc, argv, "vtsm")) != -1)
	{
		switch (argvs)
		{
		case 'v':
			std::cout << cv::getBuildInformation() << std::endl;
			break;
		case 't':
		{
		}
		break;
		case 's':
		{
		}
		break;
		case 'm':
		{
		}
		break;
		}
	}
}