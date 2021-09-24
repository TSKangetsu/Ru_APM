#pragma once
#include <thread>
#include "UORBMessage.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"
#define APMPublicFreq 250.f

class APMController_t : private SingleAPMAPI::RPiSingleAPM
{
public:
	APMController_t();
	void APMControllerConfigUpdate();
	void APMControllerRequireReboot();
	bool APMContollerIsRunning() { return IsControllerRunning; };
	~APMController_t();

	bool IsControllerRunning = false;

private:
	void APMMessageUpdate();

	std::thread RebootRequire;
	bool IsAPMReporterRunning = false;
};

//==============================================================================================//

APMController_t::APMController_t()
{
	IsControllerRunning = false;
	RPiSingleAPMInit(RuAPSSys::ConfigCLA::APMConfig);
	RPiSingleAPMStartUp();
	IsControllerRunning = true;
	APMMessageUpdate();
	TaskThreadBlock();
}

void APMController_t::APMControllerConfigUpdate()
{
	if (IsControllerRunning)
	{
		RPiSingleAPMHotLoad(RuAPSSys::ConfigCLA::APMConfig);
	}
}

void APMController_t::APMControllerRequireReboot()
{
	if (IsControllerRunning)
	{
		if (RebootRequire.joinable() == false)
		{
			RebootRequire = std::thread(
				[&]
				{
					IsControllerRunning = false;
					RPiSingleAPMDeInit();
					RPiSingleAPMInit(RuAPSSys::ConfigCLA::APMConfig);
					APMMessageUpdate();
					IsControllerRunning = true;
				});
		}
		else
			RebootRequire.join();
	}
}

void APMController_t::APMMessageUpdate(){

};

APMController_t::~APMController_t()
{
}
