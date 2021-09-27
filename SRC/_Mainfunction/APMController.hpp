#pragma once
#include <thread>
#include "UORBMessage.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"
#define APMPublicFreq 1000.f
using UORB = RuAPSSys::UORBMessage;

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

	std::thread APMTaskThread;
	std::thread RebootRequire;
	bool IsAPMReporterRunning = false;
	std::unique_ptr<FlowThread> UpdateThreading;
};

//==============================================================================================//

APMController_t::APMController_t()
{
	IsControllerRunning = false;
	RPiSingleAPMInit(RuAPSSys::ConfigCLA::APMConfig);
	RPiSingleAPMStartUp();
	APMTaskThread = std::thread([&]
								{ TaskThreadBlock(); });
	APMMessageUpdate();
	IsControllerRunning = true;
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
					UpdateThreading->FlowStopAndWait();
					//
					RPiSingleAPMDeInit();
					APMTaskThread.join();
					//
					RPiSingleAPMInit(RuAPSSys::ConfigCLA::APMConfig);
					RPiSingleAPMStartUp();
					APMTaskThread = std::thread([&]
												{ TaskThreadBlock(); });
					//
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
	UpdateThreading.reset(new FlowThread(
		[&]()
		{
			UORB::ControllerStatus.IsControllerStatusUpdated = false;
			UORB::ControllerStatus._Accel_ClippedTimes = SF._uORB_MPU_Data._uORB_MPU9250_ACC_Clipped;
			UORB::ControllerStatus._Accel_Accelration[0] = SF._uORB_MPU_Data._uORB_Acceleration_X;
			UORB::ControllerStatus._Accel_Accelration[1] = SF._uORB_MPU_Data._uORB_Acceleration_Y;
			UORB::ControllerStatus._Accel_Accelration[2] = SF._uORB_MPU_Data._uORB_Acceleration_Z;
			UORB::ControllerStatus._Accel_VIBE[0] = SF._uORB_MPU_Data._uORB_Accel_VIBE_X;
			UORB::ControllerStatus._Accel_VIBE[1] = SF._uORB_MPU_Data._uORB_Accel_VIBE_Y;
			UORB::ControllerStatus._Accel_VIBE[2] = SF._uORB_MPU_Data._uORB_Accel_VIBE_Z;
			UORB::ControllerStatus._Accel_RawG[0] = SF._uORB_MPU_Data._uORB_MPU9250_ADF_X;
			UORB::ControllerStatus._Accel_RawG[1] = SF._uORB_MPU_Data._uORB_MPU9250_ADF_Y;
			UORB::ControllerStatus._Accel_RawG[2] = SF._uORB_MPU_Data._uORB_MPU9250_ADF_Z;
			UORB::ControllerStatus._Accel_GForce = SF._uORB_MPU_Data._uORB_MPU9250_A_Vector;
			UORB::ControllerStatus._ATT_Quaterion[0] = SF._uORB_MPU_Data._uORB_Raw_QuaternionQ[0];
			UORB::ControllerStatus._ATT_Quaterion[1] = SF._uORB_MPU_Data._uORB_Raw_QuaternionQ[1];
			UORB::ControllerStatus._ATT_Quaterion[2] = SF._uORB_MPU_Data._uORB_Raw_QuaternionQ[2];
			UORB::ControllerStatus._ATT_Quaterion[3] = SF._uORB_MPU_Data._uORB_Raw_QuaternionQ[3];
			UORB::ControllerStatus._ATT_EulerAngle[0] = SF._uORB_MPU_Data._uORB_Real__Roll;
			UORB::ControllerStatus._ATT_EulerAngle[1] = SF._uORB_MPU_Data._uORB_Real_Pitch;
			UORB::ControllerStatus._ATT_EulerAngle[2] = SF._uORB_MPU_Data._uORB_Real___Yaw;
			UORB::ControllerStatus._Gyro_AngleRate[0] = SF._uORB_MPU_Data._uORB_Gryo__Roll;
			UORB::ControllerStatus._Gyro_AngleRate[1] = SF._uORB_MPU_Data._uORB_Gryo_Pitch;
			UORB::ControllerStatus._Gyro_AngleRate[2] = SF._uORB_MPU_Data._uORB_Gryo___Yaw;
			UORB::ControllerStatus._Baro_Temp = SF._uORB_BARO_Data.TemperatureC;
			UORB::ControllerStatus._Baro_PressureHPA = SF._uORB_BARO_Data.PressureHPA;
			UORB::ControllerStatus._Baro_AGLAltitudeCM = SF._uORB_BARO_Altitude;
			UORB::ControllerStatus._SYS_PreARMFlag = AF._flag_PreARM_Check_Level;
			UORB::ControllerStatus._SYS_FailSafeFlag = AF._flag_FailedSafe_Level;
			UORB::ControllerStatus._SYS_APMStatus = DF.APMStatus;
			UORB::ControllerStatus._NAV_Speed[0] = SF._uORB_True_Speed_X;
			UORB::ControllerStatus._NAV_Speed[1] = SF._uORB_True_Speed_Y;
			UORB::ControllerStatus._NAV_Speed[2] = SF._uORB_True_Speed_Z;
			UORB::ControllerStatus._NAV_Global_Speed[0] = 0;
			UORB::ControllerStatus._NAV_Global_Speed[1] = 0;
			UORB::ControllerStatus._NAV_Relative_Pos[0] = SF._uORB_True_Movement_X;
			UORB::ControllerStatus._NAV_Relative_Pos[1] = SF._uORB_True_Movement_Y;
			UORB::ControllerStatus._NAV_Relative_Pos[2] = SF._uORB_True_Movement_Z;
			UORB::ControllerStatus._NAV_Relative_Head = 0;
			UORB::ControllerStatus._NAV_Global_Pos[0] = SF._uORB_GPS_COR_Lat;
			UORB::ControllerStatus._NAV_Global_Pos[1] = SF._uORB_GPS_COR_Lat;
			UORB::ControllerStatus._NAV_Global_Pos[2] = 0;
			UORB::ControllerStatus._NAV_Global_HOME[0] = 0;
			UORB::ControllerStatus._NAV_Global_HOME[1] = 0;
			UORB::ControllerStatus._NAV_Global_SATCount = SF._uORB_GPS_Data.satillitesCount;
			UORB::ControllerStatus._NAV_Global_Head = 0;
			UORB::ControllerStatus.IsControllerStatusUpdated = true;
		},
		APMPublicFreq));
};

APMController_t::~APMController_t()
{
	UpdateThreading->FlowStopAndWait();
	// Call APM Deinit And Set Status by Manual to Noficate the System Monitor
	RPiSingleAPMDeInit();

	APMTaskThread.join();

	UORB::ControllerStatus._SYS_APMStatus = -2;
	// System Controller is exit , In fact, this flag meanless
	IsControllerRunning = false;
}
