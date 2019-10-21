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
#include <Windows.h>
#endif
int main(int argc, char* argv[])
{
	Sensors::I2CSensors_GY67 emsa;
	emsa.DataGet();
	return 0;
}