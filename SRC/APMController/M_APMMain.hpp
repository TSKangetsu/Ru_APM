#include <iostream>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <signal.h>
#include "../ToolsInclude/M_Tools.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"

namespace APMMain
{
	class NetworkController
	{
	protected:
		int deviceID;
		Base::Socket* mainSock;
		std::string dataBuff;
		NetworkController();
		void ACCSSConnectionSet();
		void dataSender(std::string);
		void dataParese(std::string data , std::string dataOut[256]);
		std::string dataCreator(int deviceID, std::string data[], int dataLength);
	};

	class APMController :SingleAPMAPI::RPiSingleAPM,public NetworkController
	{
		std::thread* Network;
		std::thread* Controller;
		std::string sendOutDataBuff[5];
		SingleAPMAPI::APMSettinngs settings;
		bool APMMainThread();
	public:
		APMController();
	};
}