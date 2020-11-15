#include "M_APMMain.hpp"

APMMain::APMController::APMController() : NetworkController()
{
	configSettle("/etc/APMconfig.json", settings);
	RPiSingleAPMInit(settings);
	APMMainThread();
}

bool APMMain::APMController::APMMainThread()
{
	IMUSensorsTaskReg();
	ControllerTaskReg();
	AltholdSensorsTaskReg();
	PositionTaskReg();
	ESCUpdateTaskReg();
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
			// sendOutDataBuff[7] = std::to_string(SF._uORB_MS5611_ClimbeRate);
			// sendOutDataBuff[8] = std::to_string(SF._uORB_MS5611_Pressure);
			dataSender(dataCreator(deviceID, sendOutDataBuff, 9));
			usleep(24000);
		}
	});
	TaskThreadBlock();
	return true;
}

void APMMain::APMController::configSettle(const char *configDir, SingleAPMAPI::APMSettinngs &APMInit)
{
	std::ifstream config(configDir);
	std::string content((std::istreambuf_iterator<char>(config)),
						(std::istreambuf_iterator<char>()));
	nlohmann::json Configdata = nlohmann::json::parse(content);
	//==========================================================Device Type=======/
	APMInit.RC_Type = Configdata["Type_RC"].get<int>();
	APMInit.MPU9250_Type = Configdata["Type_MPU9250"].get<int>();
	APMInit.IMUFilter_Type = Configdata["Type_IMUFilter"].get<int>();
	APMInit.IMUMixFilter_Type = Configdata["Type_IMUMixFilter"].get<int>();

	APMInit.__RCDevice = Configdata["__RCDevice"].get<std::string>(),
	APMInit.__GPSDevice = Configdata["__GPSDevice"].get<std::string>(),

	APMInit._IsGPSEnable = Configdata["_IsGPSEnable"].get<int>();
	APMInit._IsFlowEnable = Configdata["_IsFlowEnable"].get<int>();
	APMInit._IsSonarEnable = Configdata["_IsSonarEnable"].get<int>();
	APMInit._IsMS5611Enable = Configdata["_IsMS5611Enable"].get<int>();
	APMInit._IsRCSafeEnable = Configdata["_IsRCSafeEnable"].get<int>();
	//==========================================================Controller cofig==/
	APMInit._flag_RC_ARM_PWM_Value = Configdata["_flag_RC_ARM_PWM_Value"].get<int>();
	APMInit._flag_RC_Min_PWM_Value = Configdata["_flag_RC_Min_PWM_Value"].get<int>();
	APMInit._flag_RC_Mid_PWM_Value = Configdata["_flag_RC_Mid_PWM_Value"].get<int>();
	APMInit._flag_RC_Max_PWM_Value = Configdata["_flag_RC_Max_PWM_Value"].get<int>();

	APMInit._flag_RCIsReserv__Roll = Configdata["_flag_RCIsReserv__Roll"].get<int>();
	APMInit._flag_RCIsReserv_Pitch = Configdata["_flag_RCIsReserv_Pitch"].get<int>();
	APMInit._flag_RCIsReserv___Yaw = Configdata["_flag_RCIsReserv___Yaw"].get<int>();
	//==========================================================ESC cofig=========/
	APMInit._flag_A1_Pin = Configdata["_flag_A1_Pin"].get<int>();
	APMInit._flag_A2_Pin = Configdata["_flag_A2_Pin"].get<int>();
	APMInit._flag_B1_Pin = Configdata["_flag_B1_Pin"].get<int>();
	APMInit._flag_B2_Pin = Configdata["_flag_B2_Pin"].get<int>();
	//==================================================================PID cofig==/
	APMInit._flag_PID_P_TAsix_Gain = Configdata["_flag_PID_P_TAsix_Gain"].get<float>();
	APMInit._flag_PID_P__Roll_Gain = Configdata["_flag_PID_P__Roll_Gain"].get<float>();
	APMInit._flag_PID_P_Pitch_Gain = Configdata["_flag_PID_P_Pitch_Gain"].get<float>();
	APMInit._flag_PID_P___Yaw_Gain = Configdata["_flag_PID_P___Yaw_Gain"].get<float>();
	APMInit._flag_PID_P_Alt_Gain = Configdata["_flag_PID_P_Alt_Gain"].get<float>();
	APMInit._flag_PID_P_GPS_Gain = Configdata["_flag_PID_P_GPS_Gain"].get<float>();

	APMInit._flag_PID_I__Roll_Gain = Configdata["_flag_PID_I__Roll_Gain"].get<float>();
	APMInit._flag_PID_I_Pitch_Gain = Configdata["_flag_PID_I_Pitch_Gain"].get<float>();
	APMInit._flag_PID_I___Yaw_Gain = Configdata["_flag_PID_I___Yaw_Gain"].get<float>();
	APMInit._flag_PID_I_Alt_Gain = Configdata["_flag_PID_I_Alt_Gain"].get<float>();
	APMInit._flag_PID_I__Roll_Max__Value = Configdata["_flag_PID_I__Roll_Max__Value"].get<float>();
	APMInit._flag_PID_I_Pitch_Max__Value = Configdata["_flag_PID_I_Pitch_Max__Value"].get<float>();
	APMInit._flag_PID_I___Yaw_Max__Value = Configdata["_flag_PID_I___Yaw_Max__Value"].get<float>();

	APMInit._flag_PID_D__Roll_Gain = Configdata["_flag_PID_D__Roll_Gain"].get<float>();
	APMInit._flag_PID_D_Pitch_Gain = Configdata["_flag_PID_D_Pitch_Gain"].get<float>();
	APMInit._flag_PID_D___Yaw_Gain = Configdata["_flag_PID_D___Yaw_Gain"].get<float>();
	APMInit._flag_PID_D_Alt_Gain = Configdata["_flag_PID_D_Alt_Gain"].get<float>();
	APMInit._flag_PID_D_GPS_Gain = Configdata["_flag_PID_D_GPS_Gain"].get<float>();

	APMInit._flag_PID_Hover_Throttle = Configdata["_flag_PID_Hover_Throttle"].get<float>();
	APMInit._flag_PID_Level_Max = Configdata["_flag_PID_Level_Max"].get<float>();
	APMInit._flag_PID_Alt_Level_Max = Configdata["_flag_PID_Alt_Level_Max"].get<float>();
	APMInit._flag_PID_GPS_Level_Max = Configdata["_flag_PID_GPS_Level_Max"].get<float>();
	//==============================================================Sensors cofig==/
	APMInit._flag_MPU9250_A_X_Cali = Configdata["_flag_MPU9250_A_X_Cali"].get<double>();
	APMInit._flag_MPU9250_A_Y_Cali = Configdata["_flag_MPU9250_A_Y_Cali"].get<double>();
	APMInit._flag_MPU9250_A_Z_Cali = Configdata["_flag_MPU9250_A_Z_Cali"].get<double>();
	APMInit._flag_Accel__Roll_Cali = Configdata["_flag_Accel__Roll_Cali"].get<double>();
	APMInit._flag_Accel_Pitch_Cali = Configdata["_flag_Accel_Pitch_Cali"].get<double>();

	APMInit._flag_MPU9250_Head_Asix = Configdata["_flag_MPU9250_Head_Asix"].get<double>();
	APMInit._flag_MPU9250_M_X_Offset = Configdata["_flag_MPU9250_M_X_Offset"].get<double>();
	APMInit._flag_MPU9250_M_Y_Offset = Configdata["_flag_MPU9250_M_Y_Offset"].get<double>();
	APMInit._flag_MPU9250_M_Z_Offset = Configdata["_flag_MPU9250_M_Z_Offset"].get<double>();
	APMInit._flag_MPU9250_M_Y_Scaler = Configdata["_flag_MPU9250_M_Y_Scaler"].get<double>();
	APMInit._flag_MPU9250_M_Z_Scaler = Configdata["_flag_MPU9250_M_Z_Scaler"].get<double>();

	APMInit._flag_QMC5883L_Head_Asix = Configdata["_flag_QMC5883L_Head_Asix"].get<double>();
	APMInit._flag_QMC5883L_M_X_Offset = Configdata["_flag_QMC5883L_M_X_Offset"].get<double>();
	APMInit._flag_QMC5883L_M_Y_Offset = Configdata["_flag_QMC5883L_M_Y_Offset"].get<double>();
	APMInit._flag_QMC5883L_M_Z_Offset = Configdata["_flag_QMC5883L_M_Z_Offset"].get<double>();
	APMInit._flag_QMC5883L_M_Y_Scaler = Configdata["_flag_QMC5883L_M_Y_Scaler"].get<double>();
	APMInit._flag_QMC5883L_M_Z_Scaler = Configdata["_flag_QMC5883L_M_Z_Scaler"].get<double>();
	//===============================================================Update cofig==/
	APMInit.IMU_Freqeuncy = Configdata["IMU_Freqeucy"].get<int>();
	APMInit.RXT_Freqeuncy = Configdata["RXT_Freqeucy"].get<int>();
	APMInit.ESC_Freqeuncy = Configdata["ESC_Freqeucy"].get<int>();
}