#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#endif
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <future>
#include "../ToolsInclude/M_Tools.hpp"
#include "../CameraInclude/M_Camera.hpp"
#include <unistd.h>

namespace APMClient
{
	class GUIClient
	{
#ifdef linux
	public:
		void RecvShow(std::string ipaddress, int localport, int remoteport);
#endif
	};

	class CliClient
	{
	public:
		std::string incommingData;
		CliClient() {};
		void Client_server();
	};
} // namespace APMClient
