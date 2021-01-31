#pragma once
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <functional>
#include <openssl/sha.h>
#include "../_Excutable/Drive_Socket.hpp"
#define DEFAULT_RECV_SIZE 512

typedef enum
{
	// 0x0：标识一个中间数据包
	WCT_MINDATA = -20,
	// 0x1：标识一个txt类型数据包
	WCT_TXTDATA = -19,
	// 0x2：标识一个bin类型数据包
	WCT_BINDATA = -18,
	// 0x8：标识一个断开连接类型数据包
	WCT_DISCONN = -17,
	// 0x8：标识一个断开连接类型数据包
	WCT_PING = -16,
	// 0xA：表示一个pong类型数据包
	WCT_PONG = -15,
	WCT_ERR = -1,
	WCT_NULL = 0
} w_com_type;

namespace MessageController
{
	void get_random_string(uint8_t *buf, uint32_t len);
	std::string StringRounder(double data, int roundCount);
	std::string dataCreator(int deviceID, std::string data[], int dataLength);
	void dataParese(std::string data, std::string *dataOut, const char splti);
	void dataParese(std::string data, std::string *dataOut, std::string splti);
	class ClientNetController
	{
	public:
		void dataSender(std::string);
		void ClientNetControllerInit();

	private:
		int deviceID;
		std::string targetIP;
		int targetPort = 10086;
		char *inputBuff;
		Socket *mainSock;
		void ACCSSConnectionSet();
		void connectionSearch();
	};

	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	std::string WebSocketCreator(char *);
	std::string WebSocketHeaderCreator(std::string base64);
	std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len);
	int WebSocketDataFrameCreator(uint8_t *data, uint32_t data_len, uint8_t *package, uint32_t package_max_len, bool is_mask, w_com_type type);
	int WebSocketDataFrameDecoder(uint8_t *data, uint32_t data_len, uint8_t *package, uint32_t package_max_len, uint32_t *package_len);
	class WebSocketServer
	{
	public:
		static void dataSender(sSocket *Target, std::string SendData, w_com_type optype);
		WebSocketServer &&WebSocketServerInit();
		WebSocketServer &&OnConnection(std::function<void(sSocket *, char *)> CustomFunction)
		{
			OnConnectionFunction = CustomFunction;
			return std::move(*this);
		};
		WebSocketServer &&OnMessage(std::function<void(sSocket *, char *, int, int)> CustomFunction)
		{
			OnMessageFunction = CustomFunction;
			return std::move(*this);
		};
		WebSocketServer &&OnDisConnection(std::function<void(sSocket *)> CustomFunction)
		{
			OnDisConnectionFunction = CustomFunction;
			return std::move(*this);
		};
		WebSocketServer &&OnError(std::function<void(sSocket *, char *, int, int)> CustomFunction)
		{
			OnErrorFunction = CustomFunction;
			return std::move(*this);
		}
		WebSocketServer &&Run()
		{
			ServerMain->Run()
				.Wait();
			return std::move(*this);
		};

	private:
		int Status;
		std::function<void(sSocket *, char *)> OnConnectionFunction;
		std::function<void(sSocket *, char *, int, int)> OnMessageFunction;
		std::function<void(sSocket *)> OnDisConnectionFunction;
		std::function<void(sSocket *, char *, int, int)> OnErrorFunction;
		SocketAsyncServer *ServerMain;
	};
} // namespace MessageController