#include <stdio.h>
#include <unistd.h>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/LinuxClient.hpp"
#include "RaspberrySensors/M_Sensors.hpp"
#include <opencv2/opencv.hpp>

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
		CameraCOM::FramePost ins;
		ins.FramePostNet(0);
	}
	else if (data_comfirm == 2000)
	{
		APMClient::GUIClient ins;
		ins.RecvShow("127.0.0.1", 18800, 10086);
	}
	else if (data_comfirm == 3000)
	{
		Sensors::SensorsInfo ins("./sensors.json");
		return 0;
	}
}