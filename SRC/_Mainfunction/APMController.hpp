#pragma once
#include <thread>
#include <systemd/sd-daemon.h>
#include "UORBMessage.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"
#define APMPublicFreq 250.f
using namespace RuAPSSys;

class APMController_t : public RuAPSSys::UORBMessage, private SingleAPMAPI::RPiSingleAPM
{
public:
	APMController_t(SingleAPMAPI::APMSettinngs InitSettings, int ControllerType);
	void APMControllerConfigUpdate(SingleAPMAPI::APMSettinngs InitSettings);
	void APMControllerRequireReboot();
	bool APMContollerIsRunning() { return IsControllerRunning; };
	~APMController_t();

	bool IsControllerRunning = false;

private:
	void APMMessageUpdate();

	std::thread RebootRequire;
	bool IsAPMReporterRunning = false;
	SingleAPMAPI::APMSettinngs APMSetttings_s;
	std::unique_ptr<FlowThread> APMMessagePublic;
};

//==============================================================================================//

APMController_t::APMController_t(SingleAPMAPI::APMSettinngs InitSettings, int ControllerType)
{
	IsControllerRunning = false;
	APMSetttings_s = InitSettings;
	RPiSingleAPMInit(APMSetttings_s);
	RPiSingleAPMStartUp();
	IsControllerRunning = true;
	APMMessageUpdate();
}

void APMController_t::APMControllerConfigUpdate(SingleAPMAPI::APMSettinngs UpdateSettings)
{
	if (IsControllerRunning)
		RPiSingleAPMHotLoad(APMSetttings_s);
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
					APMMessagePublic->FlowStopAndWait();
					APMMessagePublic.reset();
					IsControllerRunning = false;
					RPiSingleAPMDeInit();
					RPiSingleAPMInit(APMSetttings_s);
					APMMessageUpdate();
					IsControllerRunning = true;
				});
		}
		else
			RebootRequire.join();
	}
}

void APMController_t::APMMessageUpdate()
{
	APMMessagePublic.reset(
		new FlowThread(
			[&]()
			{
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Accel_X].Data = (int)SF._uORB_MPU_Data._uORB_MPU9250_ADF_X;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Accel_Y].Data = (int)SF._uORB_MPU_Data._uORB_MPU9250_ADF_Y;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Accel_Z].Data = (int)SF._uORB_MPU_Data._uORB_MPU9250_ADF_Z;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Real__Roll].Data = (int)SF._uORB_MPU_Data._uORB_Real__Roll * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Real_Pitch].Data = (int)SF._uORB_MPU_Data._uORB_Real_Pitch * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Real___Yaw].Data = (int)SF._uORB_MPU_Data._uORB_Real___Yaw * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Gyro__Roll].Data = (int)SF._uORB_MPU_Data._uORB_Gryo__Roll * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Gyro_Pitch].Data = (int)SF._uORB_MPU_Data._uORB_Gryo_Pitch * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Gyro___Yaw].Data = (int)SF._uORB_MPU_Data._uORB_Gryo___Yaw * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Leveling__Roll].Data = (int)PF._uORB_Leveling__Roll * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Leveling_Pitch].Data = (int)PF._uORB_Leveling_Pitch * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Leveling___Yaw].Data = (int)PF._uORB_Leveling___Yaw * 100;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Alt_Pressure].Data = (int)SF._uORB_MS5611_PressureFinal;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Accel_VIBE_X].Data = (int)SF._uORB_MPU_Data._uORB_Accel_VIBE_X;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Accel_VIBE_Y].Data = (int)SF._uORB_MPU_Data._uORB_Accel_VIBE_Y;
				APMMessage.APMMessageGroup[APMMessageSheet::_uORB_Accel_VIBE_Z].Data = (int)SF._uORB_MPU_Data._uORB_Accel_VIBE_Z;
				APMMessage.APMMessageGroup[APMMessageSheet::_flag_Controller_AutopilotBank].Data = (int)AF.AutoPilotMode;
				APMMessage.APMMessageGroup[APMMessageSheet::_flag_Controller_ARM].Data = (int)AF._flag_ESC_ARMED;
				APMMessage.IsAPMMessageUpdate = true;
			},
			APMPublicFreq));
};

APMController_t::~APMController_t()
{
}
