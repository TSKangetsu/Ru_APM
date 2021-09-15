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

	void COMControllerPushQ(RuAPSSys::UORBMessage::APMMessage_t);

private:
	void COMControllerReg();
	std::queue<std::string> APMMessageQ;
	std::queue<APSMessageType> APSMessageQ;
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

void COMController_t::COMControllerPushQ(RuAPSSys::UORBMessage::APMMessage_t PUSG)
{
	// APMMessageQ.push(PUSG);
	if (APMMessageQ.size() > QueueSize)
		APMMessageQ.pop();
	json OutputJSON = {};
	OutputJSON += json::object_t::value_type("type", 4200);
	for (size_t i = 0; i < APMMessageSize; i++)
	{
		if (PUSG.APMMessageGroup[i].DataType == RuAPSSys::APMMessageDataType::AT___Int)
			OutputJSON += json::object_t::value_type(PUSG.APMMessageGroup[i].Name, PUSG.APMMessageGroup[i].Data);
		else if (PUSG.APMMessageGroup[i].DataType == RuAPSSys::APMMessageDataType::AT_Float)
			OutputJSON += json::object_t::value_type(PUSG.APMMessageGroup[i].Name, (float)((float)PUSG.APMMessageGroup[i].Data / 100.f));
		else if (PUSG.APMMessageGroup[i].DataType == RuAPSSys::APMMessageDataType::AT__Bool)
			OutputJSON += json::object_t::value_type(PUSG.APMMessageGroup[i].Name, ((bool)PUSG.APMMessageGroup[i].Data));
	}
	std::string dataBuffer[3];
	dataBuffer[0] = OutputJSON.dump();
	APMMessageQ.push(MessageController::dataCreator(255, dataBuffer, 2));
}