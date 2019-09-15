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
			CameraCOM::FramePost ins;
			ins.CameraCheck(0);
			//cv::Mat ins = Base::Matcom::ImageGet("/home/pi/TestData/QJ.jpg");
			//CameraCOM::CnnCaculate cnn("/home/pi/TestData/frozen_inference_graph.xml", 
			//	"/home/pi/TestData/frozen_inference_graph.bin", 
			//	1, 
			//	0, 
			//	cv::dnn::DNN_TARGET_MYRIAD);
			//cv::Mat tmp = cnn.MatCnn(ins, 300, 300, 0.4);
			//cv::imshow("ins",tmp);
			//cv::waitKey();
		}
		break;
		}
	}
}