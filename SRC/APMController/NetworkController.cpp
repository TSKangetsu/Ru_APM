#include "M_APMMain.hpp"

APMMain::NetworkController::NetworkController()
{
	connectionSearch();
#ifdef millSocket
	mainSocks = tcpconnect(ipremote(targetIP.c_str(), targetPort, 0, now() + 1000), -1);
	if (mainSocks != NULL)
	{
		std::cout << "[APMController] connected\n";
	}
	else
	{
		std::cout << "[APMController] connection failed,reconnected....\n";
		sleep(2);
		NetworkController();
	}
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
		dataParese(std::string(dataBuff), dataStrBuff, '/');
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

void APMMain::NetworkController::connectionSearch()
{
	std::string dataStrBuff[4];
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	sockaddr_in loopback;

	if (sock == -1) {
		std::cerr << "[APMController] Could not socket\n";
	}

	std::memset(&loopback, 0, sizeof(loopback));
	loopback.sin_family = AF_INET;
	loopback.sin_addr.s_addr = INADDR_LOOPBACK;
	loopback.sin_port = htons(9);

	if (connect(sock, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == -1) {
		close(sock);
		std::cerr << "[APMController] Could not connect\n";
	}

	socklen_t addrlen = sizeof(loopback);
	if (getsockname(sock, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == -1) {
		close(sock);
		std::cerr << "[APMController] Could not getsockname\n";
	}

	close(sock);

	char buf[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0) {
		std::cerr << "[APMController] Could not inet_ntop\n";
	}
	else {
		std::cout << "[APMController] Local ip address: " << buf << "\n";
	}

#ifdef millSocket
	dataParese(std::string(buf), dataStrBuff, '.');
	std::string tmp = dataStrBuff[0];
	tmp.append(".");
	tmp.append(dataStrBuff[1]);
	tmp.append(".");
	tmp.append(dataStrBuff[2]);
	tmp.append(".");
	for (size_t i = 1; i < 256; i++)
	{
		std::string tmps = tmp;
		tmps.append(std::to_string(i));
		if (tcpconnect(ipremote(tmps.c_str(), targetPort, 0, -1), now() + 10) != NULL)
		{
			std::cout << "[APMController] loced ip " << tmps << "\n";
			targetIP = tmps;
		}
	}
#endif
}

void APMMain::NetworkController::dataSender(std::string data)
{
#ifdef millSocket
	char tmpDataBuff[4];
	std::string dataStrBuff[4];
	tcpsend(mainSocks, (char*)data.c_str(), data.size(), -1); tcpflush(mainSocks, -1);
	tcprecvuntil(mainSocks, dataBuff, sizeof(dataBuff), "/!", 2, -1);
	dataParese(std::string(dataBuff), dataStrBuff, '/');
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

void APMMain::NetworkController::dataParese(std::string data, std::string databuff[256], const char splti)
{
	std::istringstream f(data);
	std::string s;
	int count = 0;
	while (getline(f, s, splti)) {
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