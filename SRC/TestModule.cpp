#include <csignal>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include "_VisionBase/VisionBaseExec.hpp"

int main()
{
	int stop;
	VideoConfig config;
	config.Width = 640;
	config.Height = 380;
	config.CameraDevice = "/dev/video0";
	config.SubHeight = 292;
	config.SubWidth = 384;
	config.CameraDeviceSub = "/dev/video1";
	config.VideoPushMode = PushMode::RTMP;
	VideoServer myserver;
	myserver.VideoServerReInit(config);
	std::cin >> stop;
}