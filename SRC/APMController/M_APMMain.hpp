#pragma once
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../ToolsInclude/M_Tools.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"
#define DEFAULT_RECV_SIZE 512

namespace APMMain
{
	class NetworkController
	{
	protected:
		int deviceID;
		int targetPort = 10086;
		std::string targetIP;
		char *inputBuff;
		Base::Socket *mainSock;
		NetworkController();
		void ACCSSConnectionSet();
		void connectionSearch();
		void dataSender(std::string);
		void dataParese(std::string data, std::string dataOut[256], const char splti);
		std::string dataCreator(int deviceID, std::string data[], int dataLength);
	};

	class APMController : SingleAPMAPI::RPiSingleAPM, public NetworkController
	{
		std::thread *Network;
		std::thread *Controller;
		std::string sendOutDataBuff[9];
		SingleAPMAPI::APMSettinngs settings;
		bool APMMainThread();

	public:
		APMController();
	};
} // namespace APMMain