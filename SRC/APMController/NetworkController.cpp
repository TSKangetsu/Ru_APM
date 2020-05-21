#include "M_APMMain.hpp"

APMMain::NetworkController::NetworkController()
{
#ifdef millSocket
	mainSocks = tcpconnect(ipremote("192.168.137.1", 10086, 0, -1), -1);
#endif // millSocket
#ifdef linuxSocket
	mainSock = new Base::Socket();
	std::cout << "[APMController] InitNetroking.....\n";
	while (!mainSock->create())
	{
		std::cout << "[APMController] socket retry!...\n";
		sleep(2);
	}
	while (!mainSock->bind("192.168.137.240", 10086))
	{
		std::cout << "[APMController] bind retry!...\n";
		sleep(2);
	}
	while (!mainSock->connect("192.168.137.1", 10086))
	{
		std::cout << "[APMController] connection retry!...\n";
		sleep(10);
	}
#endif // linuxSocket
}

void APMMain::NetworkController::ACCSSConnectionSet()
{
#ifdef millSocket
	char tmpDataBuff[4];
	std::string dataStrBuff[4];
	tcpsend(mainSocks, "4000", sizeof("4000"), -1); tcpflush(mainSocks, -1);
	tcprecv(mainSocks, dataBuff, 4, -1);
	if (std::string(dataBuff).compare("4010\001") == 0)
	{
		tcpsend(mainSocks, "4110", sizeof("4110"), -1); tcpflush(mainSocks, -1);
		tcprecv(mainSocks, dataBuff, 8, -1);
		dataParese(std::string(dataBuff), dataStrBuff);
		if (dataStrBuff[0].compare("4111") == 0)
		{
			deviceID = std::stoi(dataStrBuff[1]);
		}
		else
		{
			std::cout << "[APMController] Connect to ACCSS error,reconnecting...\n";
		}
	}
#endif // millSocket
#ifdef linuxSocket
	mainSock->send(std::string("4000"));
	mainSock->recv(dataBuff);
	if (dataBuff.compare("4010") == 0)
	{
		mainSock->send("4110");
		mainSock->recv(dataBuff);
		dataParese(dataBuff, dataStrBuff);
		if (dataStrBuff[0].compare("4111") == 0)
		{
			deviceID = std::stoi(dataStrBuff[1]);
		}
	}
	else
	{
		std::cout << "[APMController] Connect to ACCSS error,reconnecting...\n";
	}
#endif // linuxSocket
}

void APMMain::NetworkController::dataSender(std::string data)
{
#ifdef millSocket
	char tmpDataBuff[4];
	std::string dataStrBuff[4];
	tcpsend(mainSocks, (char*)data.c_str(), data.size(), -1); tcpflush(mainSocks, -1);
	tcprecvuntil(mainSocks, dataBuff, sizeof(dataBuff), "/!", 2, -1);
	dataParese(std::string(dataBuff), dataStrBuff);
	if (dataStrBuff[0].compare("4300") == 0)
	{

	}
#endif // millSocket
#ifdef linuxSocket
	mainSock->send(data);
	mainSock->recv(dataBuff);
	if (dataBuff.compare("4300"))
	{

	}
#endif // linuxSocket
}

void APMMain::NetworkController::dataParese(std::string data, std::string databuff[256])
{
	std::istringstream f(data);
	std::string s;
	int count = 0;
	while (getline(f, s, '/')) {
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