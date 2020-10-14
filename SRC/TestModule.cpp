#include <csignal>
#include <iostream>
#include <unistd.h>
#include "_VisionBase/VisionBaseExec.hpp"

int main()
{
	VideoConfig config;
	config.Width = 640;
	config.Height = 380;
	config.CameraDevice = "/dev/video0";
	config.VideoPushMode = PushMode::RTMP;
	VideoServer myserver;
	myserver.VideoServerReInit(config);
}