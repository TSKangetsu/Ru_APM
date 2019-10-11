#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "ConfigSetter/M_Config.hpp"

#ifdef linux
#include "APM_Client/LinuxClient.hpp"
#include "AirControl/RECConnect/M_RECsetter.hpp"
#include "AirControl/RECConnect/Sbus.hpp"
#include <unistd.h>
#endif

#ifdef windows
#include <Windows.h>

#endif

int main(int argc, char* argv[])
{
	_SBUS::_SBUS SbusSet(25);
}