#pragma once
#include <iostream>
#include <unistd.h>
#include <cmath>
#include "../ToolsInclude/M_Tools.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"
#include <libmill.h>

namespace APMMain
{
	class APMNetWorkControl
	{
#define startChecksum "4000"
#define comfirmChecksum "4010"
#define IDCcomfirmsum "4110"
#define IDGetSum "4111"
#define DataCheckSum "4200"
	public:
		int deviceID;
		APMNetWorkControl(char* remoteHostIP, int remoteHostPort);
		int APMRealTimeDataSend(std::string* Data);
		~APMNetWorkControl();
	private:
		char* dataBuff = new char[256];
		char* tmpDataBuff = new char[256];
		std::string tmpString;
		std::string tmpDataString;
		ipaddr localAddr;
		ipaddr remoteAddr;
		tcpsock infoSocks;
	};

	class APMControl : SingleAPMAPI::RPiSingleAPM
	{
	public:
		APMControl();
		int APMRuingSettle();
	private:
		int DataSender();
		APMNetWorkControl* ACCSSDataSender;
		std::string* sendOutDataBuff = new std::string[9];
	};
}
