#include "MessageController.hpp"

void MessageController::dataParese(std::string data, std::string *databuff, const char splti)
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

void MessageController::dataParese(std::string data, std::string *databuff, std::string splti)
{
	size_t pos = 0;
	std::string token;
	int count = 0;
	while ((pos = data.find(splti)) != std::string::npos)
	{
		token = data.substr(0, pos);
		databuff[count] = token;
		count++;
		data.erase(0, pos + splti.length());
	}
	databuff[count] = data;
}

std::string MessageController::dataCreator(int deviceID, std::string data[], int dataSize)
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

std::string MessageController::StringRounder(double data, int roundCount)
{
	std::stringstream TmpStream;
	TmpStream << std::setprecision(roundCount) << std::fixed << std::showpoint << data;
	return TmpStream.str();
}
//===================//
void MessageController::ClientNetController::ClientNetControllerInit()
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
		sleep(2);
	}
}

void MessageController::ClientNetController::ACCSSConnectionSet()
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

void MessageController::ClientNetController::connectionSearch()
{
}

void MessageController::ClientNetController::dataSender(std::string data)
{
	if (!mainSock->Send(data.c_str(), data.size()))
	{
		mainSock->~Socket();
	}
	if (!mainSock->Recv(inputBuff, DEFAULT_RECV_SIZE))
	{
		mainSock->~Socket();
	}
}
//===================//
std::string MessageController::WebSocketCreator(char *data)
{
}

std::string MessageController::base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}

void MessageController::get_random_string(uint8_t *buf, uint32_t len)
{
	uint32_t i;
	uint8_t temp;
	srand((int)time(0));
	for (i = 0; i < len; i++)
	{
		temp = (uint8_t)(rand() % 256);
		if (temp == 0)
		{
			temp = 128;
		}
		buf[i] = temp;
	}
}

int MessageController::WebSocketDataFrameCreator(uint8_t *data, uint32_t data_len, uint8_t *package, uint32_t package_max_len, bool is_mask, w_com_type type)
{
	/*掩码*/
	uint8_t mask_key[4] = {0};
	uint8_t temp1, temp2;
	int count;
	uint32_t i, len = 0;

	if (package_max_len < 2)
		return -1;

	if (type == WCT_MINDATA)
		*package++ = 0x00;
	else if (type == WCT_TXTDATA)
		*package++ = 0x81;
	else if (type == WCT_BINDATA)
		*package++ = 0x82;
	else if (type == WCT_DISCONN)
		*package++ = 0x88;
	else if (type == WCT_PING)
		*package++ = 0x89;
	else if (type == WCT_PONG)
		*package++ = 0x8A;
	else
		return -1;

	if (is_mask)
		*package = 0x80;
	len += 1;

	if (data_len < 126)
	{
		*package++ |= (data_len & 0x7F);
		len += 1;
	}
	else if (data_len < 65536)
	{
		if (package_max_len < 4)
			return -1;
		*package++ |= 0x7E;
		*package++ = (char)((data_len >> 8) & 0xFF);
		*package++ = (uint8_t)((data_len >> 0) & 0xFF);
		len += 3;
	}
	else if (data_len < 0xFFFFFFFF)
	{
		if (package_max_len < 10)
			return -1;
		*package++ |= 0x7F;
		/*(char)((data_len >> 56) & 0xFF);   数据长度变量是 uint32_t data_len, 暂时没有那么多数据*/
		*package++ = 0;
		/*(char)((data_len >> 48) & 0xFF);*/
		*package++ = 0;
		/*(char)((data_len >> 40) & 0xFF);*/
		*package++ = 0;
		/*(char)((data_len >> 32) & 0xFF);*/
		*package++ = 0;
		/*到这里就够传4GB数据了*/
		*package++ = (char)((data_len >> 24) & 0xFF);
		*package++ = (char)((data_len >> 16) & 0xFF);
		*package++ = (char)((data_len >> 8) & 0xFF);
		*package++ = (char)((data_len >> 0) & 0xFF);
		len += 9;
	}
	/*数据使用掩码时, 使用异或解码, maskKey[4]依次和数据异或运算, 逻辑如下*/
	if (is_mask)
	{
		if (package_max_len < len + data_len + 4)
			return -1;

		/*随机生成掩码*/
		get_random_string(mask_key, sizeof(mask_key));
		*package++ = mask_key[0];
		*package++ = mask_key[1];
		*package++ = mask_key[2];
		*package++ = mask_key[3];
		len += 4;
		for (i = 0, count = 0; i < data_len; i++)
		{
			temp1 = mask_key[count];
			temp2 = data[i];
			/*异或运算后得到数据*/
			*package++ = (char)(((~temp1) & temp2) | (temp1 & (~temp2)));
			count += 1;
			/*maskKey[4]循环使用*/
			if (count >= sizeof(mask_key))
				count = 0;
		}
		len += i;
		*package = '\0';
	}
	else
	{
		/*数据没使用掩码, 直接复制数据段*/
		if (package_max_len < len + data_len)
			return -1;
		memcpy(package, data, data_len);
		package[data_len] = '\0';
		len += data_len;
	}

	return len;
}

std::string MessageController::WebSocketHeaderCreator(std::string base64)
{
	std::string Header = "HTTP/1.1 101 Switching Protocols\r\n"
						 "Upgrade: websocket\r\n"
						 "Connection : Upgrade\r\n"
						 "Sec-WebSocket-Accept:";
	Header += base64;
	Header += "\r\n\r\n";
	return Header;
}

void MessageController::WebSocketServer::WebSocketServerInit()
{
	ServerMain = new SocketAsyncServer();
	ServerMain->SocketServer("0.0.0.0", 27015, 30).OnConnection(4096, [](auto *req) {
													  int Port;
													  std::string IPAddr;
													  std::cout << "\033[32m[SocketInfo]New connection incomming :";
													  req->GetRemoteInfo(IPAddr, Port);
													  std::cout << IPAddr << ":" << Port << "\033[0m\n";
												  })
		.OnMessage([](auto *req, auto *data) {
			int Port;
			std::string IPAddr;
			req->GetRemoteInfo(IPAddr, Port);

			bool IsFirstConnect = false;
			std::string mydata = data;
			unsigned char Hash[256];
			std::string base64Client;
			std::string databuff[25];
			//
			MessageController::dataParese(mydata, databuff, "\r\n");
			for (size_t i = 0; i < 25; i++)
			{
				if (strncmp(databuff[i].c_str(), "Sec-WebSocket-Key", 17) == 0)
				{
					mydata = databuff[i];
					IsFirstConnect = true;
					break;
				}
			}
			//
			if (IsFirstConnect)
			{

				MessageController::dataParese(mydata, databuff, ":");
				databuff[1] += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				databuff[1].erase(0, 1);
				//
				unsigned char *HashDataTmp = (unsigned char *)databuff[1].c_str();
				SHA1(HashDataTmp, strlen((char *)HashDataTmp), Hash);
				base64Client = MessageController::base64_encode(Hash, strlen((const char *)Hash));
				//
				req->Send(MessageController::WebSocketHeaderCreator(base64Client).c_str(), strlen(MessageController::WebSocketHeaderCreator(base64Client).c_str()));
			}
			else
			{
				std::cout << "\033[32m[SocketInfo]DataIncoming From " << IPAddr << ":" << Port << " is\n";
				int size = strlen(data);
				for (size_t i = 0; i < size; i++)
				{

					std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
				}
				std::cout << "\033[0m\n";
				uint8_t SendingData[127] = {0};
				std::string OutputData = "4300/OK!";
				int len = WebSocketDataFrameCreator((uint8_t *)OutputData.c_str(), OutputData.size(), SendingData, 512, false, WCT_TXTDATA);
				std::cout << "\033[32m[SocketInfo]SendingBack Data  " << IPAddr << ":" << Port << " is\n";
				for (size_t i = 0; i < len; i++)
				{
					std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)SendingData[i] << " ";
				}
				req->Send(SendingData, len);
			}
		})
		.OnDisConnect([](auto *req) {
			int Port;
			std::string IPAddr;
			req->GetRemoteInfo(IPAddr, Port);
			std::cout << "\033[31m[SocketInfo]Client Disconnect from " << IPAddr << ":" << Port << "\033[0m\n";
		})
		.Run()
		.Wait();
}
