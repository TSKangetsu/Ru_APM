#pragma once
#include "UORBMessage.hpp"
#include "CFGController.hpp"
#include "APMController.hpp"
#include "COMController.hpp"
#include "../_Excutable/FlowController.hpp"
#define FileConfigTarget "/etc/APMconfig.json"
#define MutilpleControllerIsNotSet 0
#define IsServerSiteEnable true
#define IsClientSiteEnable false
#define FlowExchangeHZ 100.f
#define FlowSytemMonHZ 50.f

namespace RuAPSSys
{
	class SchedulerController : public UORBMessage
	{
	public:
		SchedulerController()
		{
			// Init Step 1 : Read config from static file
			CFG::ParseConfigFromJSONFile(FileConfigTarget);
			// Init Step 2 : Boot up APMController System with static file config
			if (CFG::APSConfig.IsAPMConifgIsSet)
				APMController.reset(new APMController_t(CFG::APSConfig.APMConfig, MutilpleControllerIsNotSet));
			// Init Step 3 : Boot up and start a server to listen message from socket/TCP
			COMController.reset(new COMController_t(IsServerSiteEnable, IsClientSiteEnable));
			// Init Step 4 : Load cpp shared lib plugin

			// Init Step 5 : Load high case user script, such as LUA or Python
		};

		void SystemReboot();

		SchedulerController &&SystemMonitorReg(); //SystemMonitor will check system Status 50HZ and Report Log to file

	private:
		std::unique_ptr<FlowThread> SystemExchangeThread;
		std::unique_ptr<FlowThread> SystemMonitoThread;
		std::unique_ptr<APMController_t> APMController;
		std::unique_ptr<COMController_t> COMController;
	};
}

RuAPSSys::SchedulerController &&RuAPSSys::SchedulerController::SystemMonitorReg()
{
	SystemExchangeThread.reset(
		new FlowThread(
			[&]()
			{
				// Exchange Step 1 : Sync APMMessage and APSMessage to Scheduler Main
				if (APMController->APMMessage.IsAPMMessageUpdate)
				{
					APMMessage = APMController->APMMessage;
					APMController->APMMessage.IsAPMMessageUpdate = false;
				}
				//TODO : APSMessage network feedback sync

				// Exchange Step 2 : Push APMController's message to COMController, and reset update statment , whole thread run in 100HZ
				COMController->COMControllerPushQ(APMMessage);
			},
			FlowExchangeHZ));
	//
	SystemMonitoThread.reset(
		new FlowThread(
			[&]() {

			},
			FlowSytemMonHZ));
	//
	usleep(-1);
	return std::move(*this);
};

void RuAPSSys::SchedulerController::SystemReboot(){

};