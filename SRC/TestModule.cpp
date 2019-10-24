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
	cv::Mat Tester[6];
	for (int i = 0; i < 7; i++)
	{
		Tester[i] = cv::Mat();
	}
	cv::Mat FrameCatch;
	cv::Mat Deal;
	int ContourFound[6];
	int MaxContourFound;

	std::thread FrameCatcher([&] {
		emTest.FramePostAsync(5);
		});

	std::thread FrameDealer([&] {
		sleep(2);
		while (true)
		{
			if (!emTest.AsyncCamBuffer.empty())
			{
				for (int i = 0; i < 6; i++)
				{
					Tester[i] = emTest.AsyncCamBuffer.front();
					sleep(0.5);
				}
				for (int i = 0; i < 6; i++)
				{
					Deal = emTest.AsyncCamBuffer.front();
					Deal = Marker.ColorCut(Deal);
					ContourFound[i] = Marker.ImgMarkOut(Deal);
				}
				for (int i = 0; i < 6; i++)
				{
					int tmp;
					if (ContourFound[i] < ContourFound[i - 1])
					{
						tmp = ContourFound[i];
						ContourFound[i - 1] = ContourFound[i];
						ContourFound[i] = tmp;
					}
				}
				MaxContourFound = ContourFound[6];
				std::cout << MaxContourFound;
			}
			sleep(1);
		}
		});

	while (true)
	{
		if (!emTest.AsyncCamBuffer.empty())
		{
			FrameCatch = emTest.AsyncCamBuffer.front();
			if (emTest.AsyncCamBuffer.frameCount == 5)
				emTest.AsyncCamBuffer.clearBuffer();
			cv::imshow("test", FrameCatch);
			if (cv::waitKey(10) == 'q')
				break;
		}
	}
}