#pragma once
#include <queue>
#include <string>
#include <csignal>
#include <sstream>
#include <iostream>
#include "UORBMessage.hpp"
#include "APMController.hpp"
#include "PLGController.hpp"
#include "VIDController.hpp"
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
			// Step 1. Read Config from /boot/APSconfig.json.
			RuAPSSys::ConfigFileSync(FileConfigTarget);
			// Step 2. Load up SingleController.
			APMController.reset(new APMController_t());
			// Step 3. Load up Camera System.
			VIDController.reset(new VIDController_t());
			// ...
			// Step N. Start up all process complete.
			LOG::LogPrintSTDIO(_SYS << STARTUPCOMPLETE);
		};

		void SystemReboot();

		SchedulerController &&SystemMonitorReg(); //SystemMonitor will check system Status 50HZ and Report Log to file

	private:
		std::unique_ptr<FlowThread> SystemMonitoThread;
		std::unique_ptr<APMController_t> APMController;
		std::unique_ptr<PLGController_t> PLGController;
		std::unique_ptr<VIDController_t> VIDController;
	};
}

RuAPSSys::SchedulerController &&RuAPSSys::SchedulerController::SystemMonitorReg()
{
	SystemMonitoThread.reset(
		new FlowThread(
			[&]()
			{
				// Check message queue from other thread and print
				if (UORB::SystemStatus.SystemMessage.size() > 0)
				{
					LOG::LogPrintSTDIO(UORB::SystemStatus.SystemMessage.front());
					UORB::SystemStatus.SystemMessage.pop();
				}

				// Stop Signal Handle Check.
				if (APSSystemSignal == SIGTERM || APSSystemSignal == SIGINT)
				{
					// Call APMControll Stop all threads.
					LOG::LogPrintSTDIO(_SYS << EXITPROCESSACTD);
					// Shut off all Controller
					APMController.reset(); // This Will Block untill APM complete Stop.
					LOG::LogPrintSTDIO(_APM << APMEXITPROCESSD);
					VIDController.reset();
					LOG::LogPrintSTDIO(_VID << VIDEXITPROCESSD);
					// Exiting whole progress
					LOG::LogPrintSTDIO(_SYS << SYSTEMEXITEDCAL);
					exit(0);
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
	LOG::LogPrintSTDIO(_SYS << SIGNALRECVINPUT << Signal << "\n");
}
