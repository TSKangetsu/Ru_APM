#include "M_APMMain.hpp"

APMMain::NetworkController::NetworkController()
{
	connectionSearch();
	mainSock = new Socket();
	std::cout << "[APMController] InitNetroking.....\n";
	while (!mainSock->Create())
	{
		std::cout << "[APMController] socket retry!...\n";
		sleep(2);
	}
	while (!mainSock->Bind("0.0.0.0", 10086))
	{
		std::cout << "[APMController] bind retry!...\n";
		sleep(2);
	}
	while (!mainSock->Connect("192.168.137.1", 10086))
	{
		std::cout << "[APMController] connection retry!...\n";
		sleep(10);
	}
}

void APMMain::NetworkController::ACCSSConnectionSet()
{
	std::string dataStrBuff[10];
	mainSock->Send(std::string("4000").c_str(), std::string("4000").size());
	mainSock->Recv(inputBuff, DEFAULT_RECV_SIZE);
	if (strcmp(inputBuff, "4010") == 0)
	{
		mainSock->Send(std::string("4110").c_str(), std::string("4110").size());
		mainSock->Recv(inputBuff, DEFAULT_RECV_SIZE);
		dataParese(inputBuff, dataStrBuff, '/');
		if (dataStrBuff[0].compare("4111") == 0)
		{
			deviceID = std::stoi(dataStrBuff[1]);
		}
	}
	else
	{
		std::cout << "[APMController] Connect to ACCSS error,reconnecting...\n";
	}
}

void APMMain::NetworkController::connectionSearch()
{
}

void APMMain::NetworkController::dataSender(std::string data)
{
	mainSock->Send(data.c_str(), data.size());
	mainSock->Recv(inputBuff, DEFAULT_RECV_SIZE);
	if (inputBuff == "4300")
	{
	}
}

void APMMain::NetworkController::dataParese(std::string data, std::string databuff[256], const char splti)
{
	std::istringstream f(data);
	std::string s;
	int count = 0;
	while (getline(f, s, splti))
	{
		databuff[count] = s;
		count++;
	}
}

std::string APMMain::NetworkController::dataCreator(int deviceID, std::string data[], int dataSize)
{
	std::string tmpDataString = "4200";
	tmpDataString += "/";
	tmpDataString += std::to_string(deviceID);
	tmpDataString += "/";
	for (size_t i = 0; i < dataSize; i++)
	{
		tmpDataString += data[i];
		tmpDataString += "/";
	}
	return tmpDataString;
}