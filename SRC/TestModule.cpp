#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/LinuxClient.hpp"
#include "RaspberrySensors/M_Sensors.hpp"

int main(int argc, char* argv[])
{
	int argvs;
	int data_comfirm;
	while ((argvs = getopt(argc, argv, "vtsm")) != -1)
	{
		switch (argvs)
		{
		case 'v':
			std::cout << cv::getBuildInformation() << std::endl;
			break;
		case 't':
			data_comfirm = 1000;
			break;
		case 's':
			data_comfirm = 2000;
			break;
		case 'm':
			data_comfirm = 3000;
			break;
		}
	}
	if (data_comfirm == 1000)
	{
	}
	else if (data_comfirm == 2000)
	{
	}
	else if (data_comfirm == 3000)
	{
		CameraCOM::CnnCaculate ins;
		ins.MatCnnRaspi(300, 300);
	}
}