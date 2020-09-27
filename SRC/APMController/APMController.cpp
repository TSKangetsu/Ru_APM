#include "M_APMMain.hpp"

APMMain::APMController::APMController() : NetworkController()
{
	RPiSingleAPMInit(settings);
	APMMainThread();
}

bool APMMain::APMController::APMMainThread()
{
	IMUSensorsTaskReg();
	ControllerTaskReg();
	ESCUpdateTaskReg();
	AltholdSensorsTaskReg();
	Network = new std::thread([&] {
		ACCSSConnectionSet();
		while (true)
		{
			sendOutDataBuff[0] = std::to_string(SF._uORB_Gryo_Pitch);
			sendOutDataBuff[1] = std::to_string(SF._uORB_Gryo__Roll);
			sendOutDataBuff[2] = std::to_string(SF._uORB_Gryo___Yaw);
			sendOutDataBuff[3] = std::to_string(SF._uORB_Accel_Pitch);
			sendOutDataBuff[4] = std::to_string(SF._uORB_Accel__Roll);
			sendOutDataBuff[5] = std::to_string(SF._uORB_Real_Pitch);
			sendOutDataBuff[6] = std::to_string(SF._uORB_Real__Roll);
			sendOutDataBuff[7] = std::to_string(SF._uORB_MS5611_ClimbeRate);
			sendOutDataBuff[8] = std::to_string(SF._uORB_MS5611_Pressure);
			dataSender(dataCreator(deviceID, sendOutDataBuff, 9));
			usleep(24000);
		}
	});
	TaskThreadBlock();
	return true;
}
