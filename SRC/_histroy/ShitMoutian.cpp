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
#include "AirControl/RECConnect/RuStartSong.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#endif

#ifdef windows
#include <io.h>
#include <Windows.h>
#endif

int PWMTime;
bool startingMarking = false;
bool Markfinish = false;
struct timeval ts, te;

void Markout()
{
	cv::namedWindow("Test", cv::WINDOW_NORMAL);
	cv::setWindowProperty("Test", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
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
			if (PWMTime < 2100 && PWMTime >1700)
			{
				sleep(1);
				if (PWMTime < 2100 && PWMTime >1700)
				{
					startingMarking = true;
					digitalWrite(26, HIGH);
					sleep(5);
					digitalWrite(26, LOW);
					if (!emTest.AsyncCamBuffer.empty())
					{
						Deal = emTest.AsyncCamBuffer.front();
						Deal = Marker.ColorCut(Deal);
						ContourFound = Marker.ImgMarkOut(Deal);
						std::cout << "  contourfound:" << ContourFound << "\n";
						for (int i = 0; i < 5; i++)
						{
							for (int i = 0; i < ContourFound; i++)
							{
								digitalWrite(7, LOW);
								usleep(200000);
								digitalWrite(7, HIGH);
								usleep(200000);
							}
							sleep(1);
						}
						startingMarking = false;
						Markfinish = true;
						sleep(1);
						Markfinish = false;
					}
				}
			}
			sleep(1);
		}
		});

	while (true)
	{
		if (!emTest.AsyncCamBuffer.empty())
		{
			FrameCatch = emTest.AsyncCamBuffer.front();
			if (PWMTime < 2100 && PWMTime >1700)
			{
				cv::putText(FrameCatch, "Start to Detecting...", cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0));
			}
			if (Markfinish)
			{
				cv::putText(FrameCatch, "Marking Finish", cv::Point(10, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
			}
			if (startingMarking)
			{
				cv::putText(FrameCatch, "makring start", cv::Point(10, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));
			}
			cv::imshow("Test", FrameCatch);
			if (cv::waitKey(10) == 'q')
				break;
		}
	}
}

void PWMread()
{
	int value = digitalRead(0);
	if (value == 1)
	{
		gettimeofday(&ts, NULL);
	}
	gettimeofday(&te, NULL);
	PWMTime = (te.tv_sec - ts.tv_sec) * 1000000 + (te.tv_usec - ts.tv_usec);
}

void wiringPisuckUp()
{
	piHiPri(99);
	if (wiringPiSetup() == -1) {
		printf("[WiringPiStatus] WiringPi initialization failed !");
	}
	std::cout << "[WiringPiStatus] WiringPI SetSuccessfull !" << "\n";

	//----light
	pinMode(26, OUTPUT);
	digitalWrite(26, HIGH);
	sleep(1);
	digitalWrite(26, LOW);

	//----buzzer
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);

	pinMode(0, INPUT);
	wiringPiISR(0, INT_EDGE_BOTH, PWMread);
}

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
		{
			CameraCOM::FramePost emtTest;
			emtTest.CameraOutput(0);
		}
		break;
		case 's':
		{
			Markout();
		}
		break;
		case 'm':
		{
			std::thread tmw([] {
				wiringPisuckUp();
				});
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(3, &cpuset);
			int rc = pthread_setaffinity_np(tmw.native_handle(),
				sizeof(cpu_set_t), &cpuset);
			tmw.join();
			Markout();
		}
		break;
		}
	}
}