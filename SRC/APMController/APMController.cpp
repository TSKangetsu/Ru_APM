#include "M_APMMain.hpp"

APMMain::APMController::APMController() :NetworkController()
{
	RPiSingleAPMInit(settings);
	APMMainThread();
}

bool APMMain::APMController::APMMainThread()
{
	Controller = new std::thread([&]
		{
			while (true)
			{
				IMUSensorsParse();
				ControlParse();
				AttitudeUpdate();
				SaftyChecking();
				ESCUpdate();
				ClockingTimer();
			}
		});

	Network = new std::thread([&]
		{
			ACCSSConnectionSet();
			while (true)
			{
				sendOutDataBuff[0] = std::to_string(SF._uORB_Gryo_Pitch);
				sendOutDataBuff[1] = std::to_string(SF._uORB_Gryo__Roll);
				sendOutDataBuff[2] = std::to_string(SF._uORB_Gryo___Yaw);
				sendOutDataBuff[3] = std::to_string(SF._uORB_Accel_Pitch);
				sendOutDataBuff[4] = std::to_string(SF._uORB_Accel__Roll);
				dataSender(dataCreator(deviceID, sendOutDataBuff, 5));
				usleep(24000);
			}
		});

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(3, &cpuset);
	int rc = pthread_setaffinity_np(Controller->native_handle(), sizeof(cpu_set_t), &cpuset);
	Controller->join();

	return true;
}
