#pragma once
#include <csignal>
#include "UORBMessage.hpp"
#include "APMController.hpp"
#include "PLGController.hpp"
#include "VideoController.hpp"
#include "../_Excutable/FlowController.hpp"

#define FileConfigTarget "/boot/APSconfig.json"
#define IsServerSiteEnable true
#define IsClientSiteEnable false
#define FlowExchangeHZ 250.f
#define FlowSytemMonHZ 50.f

using UORB = RuAPSSys::UORBMessage;

namespace RuAPSSys
{
	inline volatile std::sig_atomic_t APSSystemSignal;

	class SchedulerController
	{
	public:
		SchedulerController()
		{
			// Step 1. Read Config from /boot/APSconfig.json
			RuAPSSys::ConfigFileSync(FileConfigTarget);
			// Step 2. Load up SingleController
			APMController.reset(new APMController_t());
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
					APMController.reset(); // This Will Block untill APM complete Stop.
					if (UORB::ControllerStatus._SYS_APMStatus == -2)
						exit(0);
				}
			},
			FlowSytemMonHZ));
	usleep(-1);
	return std::move(*this);
};

void RuAPSSys::SchedulerController::SystemReboot(){

};