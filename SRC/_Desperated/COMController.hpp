#pragma once
#include <queue>
#include "UORBMessage.hpp"
#include "../_Excutable/Drive_Json.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../MessageController/MessageController.hpp"
using json = nlohmann::json;
#define QueueSize 10
#define APMMessageSize 17

class COMController_t : public RuAPSSys::UORBMessage
{
public:
	COMController_t(bool ServerEnable, bool ClientEnable);
	~COMController_t();

private:
	void COMControllerReg();
	std::queue<std::string> APMMessageQ;
	std::unique_ptr<MessageController::WebSocketServer> ServerSite;
	std::unique_ptr<MessageController::ClientNetController> ClientSite;
};

//==============================================================================================//

COMController_t::COMController_t(bool ServerEnable, bool ClientEnable)
{
	if (ServerEnable)
		ServerSite.reset(new MessageController::WebSocketServer());
	COMControllerReg();
}

void COMController_t::COMControllerReg()
{
	ServerSite->WebSocketServerInit()
		.OnConnection(
			[&](auto *req, auto *data)
			{
				std::string output = "4000";
				MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
			})
		.OnMessage(
			[&](auto *req, auto *data, auto opcode, auto len)
			{
				std::string output;
				if (strncmp(data, "4010", 4) == 0)
				{
					output = "4110";
					MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
				}
				if (strncmp(data, "4110", 4) == 0)
				{
					output = "4112";
					MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
				}
				if (strncmp(data, "4350", 4) == 0)
				{
					output = APMMessageQ.front();
					MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
				}
				if (strncmp(data, "4444", 4) == 0)
				{
					//TODO: APMMessage RollIn.
				}
			})
		.OnDisConnection(
			[&](auto *req)
			{ return; })
		.OnError(
			[&](auto *req, auto *data, auto opcode, auto len)
			{
				std::string output = "4300/ERROR!";
				MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
			})
		.Run();
	;
}

COMController_t::~COMController_t()
{
	ServerSite.reset();
	ClientSite.reset();
}