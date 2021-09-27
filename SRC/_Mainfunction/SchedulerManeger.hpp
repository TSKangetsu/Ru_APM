#pragma once
#include <queue>
#include <string>
#include <csignal>
#include <sstream>
#include <iostream>
#include "UORBMessage.hpp"
#include "APMController.hpp"
#include "PLGController.hpp"
#include "VideoController.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../_Excutable/LogPublicator.hpp"

#define FileConfigTarget "/boot/APSconfig.json"
#define IsServerSiteEnable true
#define IsClientSiteEnable false
#define FlowExchangeHZ 250.f
#define FlowSytemMonHZ 50.f

using UORB = RuAPSSys::UORBMessage;
namespace LOG = LOGPublicator;

namespace RuAPSSys
{
	void SignalCatcher(int Signal);
	//
	inline static int APSSystemSignal;
	inline std::queue<std::string> SystemMessage;
	//
	class SchedulerController
	{
	public:
		SchedulerController()
		{
			// Step 1. Read Config from /boot/APSconfig.json
			RuAPSSys::ConfigFileSync(FileConfigTarget);
			// Step 2. Load up SingleController
			APMController.reset(new APMController_t());
			LOG::LogPrintSTDIO(
				StringBuilder()
				<< "[RuAPS-SYS] ALL Start Process complete\n");
		};

		void SystemReboot();

		SchedulerController &&SystemMonitorReg(); //SystemMonitor will check system Status 50HZ and Report Log to file

	private:
		std::unique_ptr<FlowThread> SystemMonitoThread;
		std::unique_ptr<APMController_t> APMController;
		std::unique_ptr<PLGController_t> PLGController;
	};
}

RuAPSSys::SchedulerController &&RuAPSSys::SchedulerController::SystemMonitorReg()
{
	SystemMonitoThread.reset(
		new FlowThread(
			[&]()
			{
				// Stop Signal Handle Check.
				if (APSSystemSignal == SIGTERM || APSSystemSignal == SIGINT)
				{
					// Call APMControll Stop all threads.
					LOG::LogPrintSTDIO(
						StringBuilder()
						<< "[RuAPS-SYS] Terminal request revice, Exiting Procession is Active....\n");
					//
					APMController.reset(); // This Will Block untill APM complete Stop.

					LOG::LogPrintSTDIO(
						StringBuilder()
						<< "[RuAPS-APM] APMContoroller Exited.\n");
					//
					if (UORB::ControllerStatus._SYS_APMStatus == -2)
					{
						LOG::LogPrintSTDIO(
							StringBuilder()
							<< "[RuAPS-SYS] System Exiting Procession Complete.\n");
						exit(0);
					}
				}
			},
			FlowSytemMonHZ));
	// Call Signal Handler
	std::signal(SIGINT, SignalCatcher);
	std::signal(SIGTERM, SignalCatcher);
	// Block Runner life
	SystemMonitoThread->FlowWait();
	return std::move(*this);
};

void RuAPSSys::SchedulerController::SystemReboot(){

};

void RuAPSSys::SignalCatcher(int Signal)
{
	RuAPSSys::APSSystemSignal = Signal;
	std::cout << "[RuAPS-SYS] System interrupt with:" << Signal << "\n";
}
