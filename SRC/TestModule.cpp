#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <iostream>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/ComClient.hpp"
#include "ConfigSetter/M_Config.hpp"
#ifdef linux
#include "AirControl/RECConnect/M_RECsetter.hpp"
#include "AirControl/SensorsConnet/SensorsDevice.hpp"
#include <unistd.h>
#include <time.h>
#endif

#ifdef windows
#include <io.h>
#include <Windows.h>
#endif
int main(int argc, char* argv[])
{
	CameraCOM::FramePost emTest;
	CameraCOM::MarkOutModule Marker;
	cv::Mat FrameCatch;
	cv::Mat Deal;
	int ContourFound;

	std::thread FrameCatcher([&] {
		emTest.FramePostAsync(5);
		});

	std::thread FrameDealer([&] {
		while (true)
		{
			if (!emTest.AsyncCamBuffer.empty())
			{
				Deal = emTest.AsyncCamBuffer.front();
				Deal = Marker.ColorCut(Deal);
				ContourFound = Marker.ImgMarkOut(Deal);
#define DEBUG
#ifdef DEBUG
				std::cout << "contourfound:" << ContourFound << "\n";
#endif
				cv::imshow("x", Deal);
				if (cv::waitKey(10) == 'q')
					break;
				Sleep(200);

			}
		}
		});

	while (true)
	{
		if (!emTest.AsyncCamBuffer.empty())
		{
			FrameCatch = emTest.AsyncCamBuffer.front();
			cv::imshow("test", FrameCatch);
			if (cv::waitKey(10) == 'q')
				break;
		}
	}
}