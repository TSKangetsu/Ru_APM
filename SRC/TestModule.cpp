#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include "CameraInclude/M_Camera.hpp"
#include "ToolsInclude/M_Tools.hpp"
#include "APM_Client/LinuxClient.hpp"
#include "RaspberrySensors/M_Sensors.hpp"
#include "ConfigSetter/M_Config.hpp"
#ifdef linux
#include <unistd.h>
#endif

#ifdef windows
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
	CameraCOM::DnnModule ins;
	ins.AsyncMatDnnDeal();
	//const int sax = 100;
	// std::thread server([&]() {
	//char *data = new char[50];
	//Base::Socket socketTMP;
	//Base::Socket somc;
	//socketTMP.Create();
	//socketTMP.Bind(10087);
	//socketTMP.Listen(10);
	//socketTMP.Accept(somc);
	// });
	//somc.Recv<char>(data, sax);
	//Sleep(1000);
	//std::cout << "client start";
	//std::thread client([&]() {
	//	Base::Socket socketTmp;
	//	socketTmp.Create();
	//	socketTmp.Bind(10086);
	//	socketTmp.Connect("127.0.0.1", 10087);
	//});

	// server.join();
	//client.join();
}
