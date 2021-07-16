#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include "PLGController.hpp"
#include "../_Excutable/Drive_Json.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"

namespace CFG
{
	enum ScriptType
	{
		Lua,
		None,
		Python,
	};

	static struct APSConfig_t
	{
		bool IsAPMConifgIsSet = false;
		SingleAPMAPI::APMSettinngs APMConfig;
		//============================================================//
		struct NetWorkConfig_t
		{
			bool IsServerEnable = false;
			bool IsClientEnable = false;

			int BindOfPort = 27015;
			std::string BindOfIPAddress = "127.0.0.1";
		} NetWorkConfig;
		//
		PLGSettings PLGConfig;
		//============================================================//
		void APMConfigSet(SingleAPMAPI::APMSettinngs APMConfigIn)
		{
			APMConfig = APMConfigIn;
			IsAPMConifgIsSet = true;
		}
	} APSConfig;

	void ParseConfigFromJSON(std::string);
	void ParseConfigFromJSONFile(std::string);
}

//==============================================================================================//

void CFG::ParseConfigFromJSON(std::string JSON)
{
	APSConfig.IsAPMConifgIsSet = false;
	SingleAPMAPI::APMSettinngs APMConfigTMP;
	nlohmann::json Configdata = nlohmann::json::parse(JSON);
	//==========================================================Device Type=======/
	APMConfigTMP.DC.RC_Type = Configdata["Type_RC"].get<int>();
	APMConfigTMP.DC.MPU9250_Type = Configdata["Type_MPU9250"].get<int>();
	APMConfigTMP.DC.IMUFilter_Type = Configdata["Type_IMUFilter"].get<int>();

	APMConfigTMP.DC.__RCDevice = Configdata["__RCDevice"].get<std::string>(),
	APMConfigTMP.DC.__GPSDevice = Configdata["__GPSDevice"].get<std::string>(),
	APMConfigTMP.DC.__FlowDevice = Configdata["__FlowDevice"].get<std::string>(),

	APMConfigTMP.DC._IsGPSEnable = Configdata["_IsGPSEnable"].get<bool>();
	APMConfigTMP.DC._IsFlowEnable = Configdata["_IsFlowEnable"].get<bool>();
	APMConfigTMP.DC._IsMS5611Enable = Configdata["_IsMS5611Enable"].get<bool>();
	APMConfigTMP.DC._IsRCSafeEnable = Configdata["_IsRCSafeEnable"].get<bool>();

	APMConfigTMP.DC.IMU_Freqeuncy = Configdata["IMU_Freqeucy"].get<int>();
	APMConfigTMP.DC.RXT_Freqeuncy = Configdata["RXT_Freqeucy"].get<int>();
	APMConfigTMP.DC.ESC_Freqeuncy = Configdata["ESC_Freqeucy"].get<int>();
	//==========================================================Controller cofig==/
	APMConfigTMP.RC._flag_RC_Min_PWM_Value = Configdata["_flag_RC_Min_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_Mid_PWM_Value = Configdata["_flag_RC_Mid_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_Max_PWM_Value = Configdata["_flag_RC_Max_PWM_Value"].get<int>();
	//===========================
	APMConfigTMP.RC._flag_RC_ARM_PWM_Value = Configdata["_flag_RC_ARM_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_ARM_PWM_Channel = Configdata["_flag_RC_ARM_PWM_Channel"].get<int>();

	APMConfigTMP.RC._flag_RC_AP_ManualHold_PWM_Value = Configdata["_flag_RC_AP_ManualHold_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_AP_ManualHold_PWM_Channel = Configdata["_flag_RC_AP_ManualHold_PWM_Channel"].get<int>();

	APMConfigTMP.RC._flag_RC_AP_AutoStable_PWM_Value = Configdata["_flag_RC_AP_AutoStable_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_AP_AutoStable_PWM_Channel = Configdata["_flag_RC_AP_AutoStable_PWM_Channel"].get<int>();

	APMConfigTMP.RC._flag_RC_AP_AltHold_PWM_Value = Configdata["_flag_RC_AP_AltHold_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_AP_AltHold_PWM_Channel = Configdata["_flag_RC_AP_AltHold_PWM_Channel"].get<int>();

	APMConfigTMP.RC._flag_RC_AP_SpeedHold_PWM_Value = Configdata["_flag_RC_AP_SpeedHold_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_AP_SpeedHold_PWM_Channel = Configdata["_flag_RC_AP_SpeedHold_PWM_Channel"].get<int>();

	APMConfigTMP.RC._flag_RC_AP_PositionHold_PWM_Value = Configdata["_flag_RC_AP_PositionHold_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_AP_PositionHold_PWM_Channel = Configdata["_flag_RC_AP_PositionHold_PWM_Channel"].get<int>();

	APMConfigTMP.RC._flag_RC_AP_UserAuto_PWM_Value = Configdata["_flag_RC_AP_UserAuto_PWM_Value"].get<int>();
	APMConfigTMP.RC._flag_RC_AP_UserAuto_PWM_Channel = Configdata["_flag_RC_AP_UserAuto_PWM_Channel"].get<int>();
	//===========================
	APMConfigTMP.RC._flag_RCIsReserv__Roll = Configdata["_flag_RCIsReserv__Roll"].get<int>();
	APMConfigTMP.RC._flag_RCIsReserv_Pitch = Configdata["_flag_RCIsReserv_Pitch"].get<int>();
	APMConfigTMP.RC._flag_RCIsReserv___Yaw = Configdata["_flag_RCIsReserv___Yaw"].get<int>();
	//==========================================================ESC cofig=========/
	APMConfigTMP.OC._flag_A1_Pin = Configdata["_flag_A1_Pin"].get<int>();
	APMConfigTMP.OC._flag_A2_Pin = Configdata["_flag_A2_Pin"].get<int>();
	APMConfigTMP.OC._flag_B1_Pin = Configdata["_flag_B1_Pin"].get<int>();
	APMConfigTMP.OC._flag_B2_Pin = Configdata["_flag_B2_Pin"].get<int>();
	APMConfigTMP.OC._flag_YAWOut_Reverse = Configdata["_flag_YAWOut_Reverse"].get<float>();
	//==================================================================PID cofig==/
	APMConfigTMP.PC._flag_PID_P__Roll_Gain = Configdata["_flag_PID_P__Roll_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_Pitch_Gain = Configdata["_flag_PID_P_Pitch_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P___Yaw_Gain = Configdata["_flag_PID_P___Yaw_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_Alt_Gain = Configdata["_flag_PID_P_Alt_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_PosX_Gain = Configdata["_flag_PID_P_PosX_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_PosY_Gain = Configdata["_flag_PID_P_PosY_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_SpeedZ_Gain = Configdata["_flag_PID_P_SpeedZ_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_SpeedX_Gain = Configdata["_flag_PID_P_SpeedX_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_P_SpeedY_Gain = Configdata["_flag_PID_P_SpeedY_Gain"].get<float>();

	APMConfigTMP.PC._flag_PID_I__Roll_Gain = Configdata["_flag_PID_I__Roll_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_Pitch_Gain = Configdata["_flag_PID_I_Pitch_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I___Yaw_Gain = Configdata["_flag_PID_I___Yaw_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_Alt_Gain = Configdata["_flag_PID_I_Alt_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_PosX_Gain = Configdata["_flag_PID_I_PosX_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_PosY_Gain = Configdata["_flag_PID_I_PosY_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_SpeedZ_Gain = Configdata["_flag_PID_I_SpeedZ_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_SpeedX_Gain = Configdata["_flag_PID_I_SpeedX_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I_SpeedY_Gain = Configdata["_flag_PID_I_SpeedY_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_I__Roll_Max__Value = Configdata["_flag_PID_I__Roll_Max__Value"].get<float>();
	APMConfigTMP.PC._flag_PID_I_Pitch_Max__Value = Configdata["_flag_PID_I_Pitch_Max__Value"].get<float>();
	APMConfigTMP.PC._flag_PID_I___Yaw_Max__Value = Configdata["_flag_PID_I___Yaw_Max__Value"].get<float>();

	APMConfigTMP.PC._flag_PID_D__Roll_Gain = Configdata["_flag_PID_D__Roll_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_D_Pitch_Gain = Configdata["_flag_PID_D_Pitch_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_D___Yaw_Gain = Configdata["_flag_PID_D___Yaw_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_D_SpeedZ_Gain = Configdata["_flag_PID_D_SpeedZ_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_D_SpeedX_Gain = Configdata["_flag_PID_D_SpeedX_Gain"].get<float>();
	APMConfigTMP.PC._flag_PID_D_SpeedY_Gain = Configdata["_flag_PID_D_SpeedY_Gain"].get<float>();

	APMConfigTMP.PC._flag_PID_Hover_Throttle = Configdata["_flag_PID_Hover_Throttle"].get<float>();
	APMConfigTMP.PC._flag_PID_Level_Max = Configdata["_flag_PID_Level_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_Alt_Level_Max = Configdata["_flag_PID_Alt_Level_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_Pos_Level_Max = Configdata["_flag_PID_Pos_Level_Max"].get<float>();

	APMConfigTMP.PC._flag_PID_Takeoff_Altitude = Configdata["_flag_PID_Takeoff_Altitude"].get<float>();
	APMConfigTMP.PC._flag_PID_Alt_Speed_Max = Configdata["_flag_PID_Alt_Speed_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_Alt_Accel_Max = Configdata["_flag_PID_Alt_Accel_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_PosMan_Speed_Max = Configdata["_flag_PID_PosMan_Speed_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_Pos_Accel_Max = Configdata["_flag_PID_Pos_Accel_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_Pos_Speed_Max = Configdata["_flag_PID_Pos_Speed_Max"].get<float>();
	APMConfigTMP.PC._flag_PID_AngleRate_Gain = Configdata["_flag_PID_AngleRate_Gain"].get<float>();
	//==============================================================Sensors cofig==/
	APMConfigTMP.SC._flag_MPU9250_A_X_Cali = Configdata["_flag_MPU9250_A_X_Cali"].get<double>();
	APMConfigTMP.SC._flag_MPU9250_A_Y_Cali = Configdata["_flag_MPU9250_A_Y_Cali"].get<double>();
	APMConfigTMP.SC._flag_MPU9250_A_Z_Cali = Configdata["_flag_MPU9250_A_Z_Cali"].get<double>();
	APMConfigTMP.SC._flag_MPU9250_A_X_Scal = Configdata["_flag_MPU9250_A_X_Scal"].get<double>();
	APMConfigTMP.SC._flag_MPU9250_A_Y_Scal = Configdata["_flag_MPU9250_A_Y_Scal"].get<double>();
	APMConfigTMP.SC._flag_MPU9250_A_Z_Scal = Configdata["_flag_MPU9250_A_Z_Scal"].get<double>();

	APMConfigTMP.SC._flag_COMPASS_Y_Scaler = Configdata["_flag_COMPASS_Y_Scaler"].get<double>();
	APMConfigTMP.SC._flag_COMPASS_Z_Scaler = Configdata["_flag_COMPASS_Z_Scaler"].get<double>();
	APMConfigTMP.SC._flag_COMPASS_X_Offset = Configdata["_flag_COMPASS_X_Offset"].get<double>();
	APMConfigTMP.SC._flag_COMPASS_Y_Offset = Configdata["_flag_COMPASS_Y_Offset"].get<double>();
	APMConfigTMP.SC._flag_COMPASS_Z_Offset = Configdata["_flag_COMPASS_Z_Offset"].get<double>();
	//==============================================================Filter config==/
	APMConfigTMP.FC._flag_Filter_Gryo_Type = Configdata["_flag_Filter_Gryo_Type"].get<double>();
	APMConfigTMP.FC._flag_Filter_GYaw_CutOff = Configdata["_flag_Filter_GYaw_CutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_Gryo_CutOff = Configdata["_flag_Filter_Gryo_CutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_Gryo_NotchFreq = Configdata["_flag_Filter_Gryo_NotchFreq"].get<double>();
	APMConfigTMP.FC._flag_Filter_Gryo_NotchCutOff = Configdata["_flag_Filter_Gryo_NotchCutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_Gryo_DynamicNotchRange = Configdata["_flag_Filter_Gryo_DynamicNotchRange"].get<double>();
	APMConfigTMP.FC._flag_Filter_Gryo_DynamicNotchEnable = Configdata["_flag_Filter_Gryo_DynamicNotchEnable"].get<bool>();
	APMConfigTMP.FC._flag_Filter_Gryo_DynamicNotchMinFreq = Configdata["_flag_Filter_Gryo_DynamicNotchMinFreq"].get<double>();
	APMConfigTMP.FC._flag_Filter_Accel_Type = Configdata["_flag_Filter_Accel_Type"].get<double>();
	APMConfigTMP.FC._flag_Filter_Accel_CutOff = Configdata["_flag_Filter_Accel_CutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_AngleMix_Alpha = Configdata["_flag_Filter_AngleMix_Alpha"].get<double>();

	APMConfigTMP.FC._flag_Baro_Trust_Beta = Configdata["_flag_Baro_Trust_Beta"].get<double>();
	APMConfigTMP.FC._flag_Accel_Trust_Beta = Configdata["_flag_Accel_Trust_Beta"].get<double>();
	APMConfigTMP.FC._flag_Sonar_Trust_Beta = Configdata["_flag_Sonar_Trust_Beta"].get<double>();
	APMConfigTMP.FC._flag_GPSAlt_Trust_Beta = Configdata["_flag_GPSAlt_Trust_Beta"].get<double>();
	APMConfigTMP.FC._flag_AccelBias_Trust_Beta = Configdata["_flag_AccelBias_Trust_Beta"].get<double>();

	APMConfigTMP.FC._flag_Filter_RC_CutOff = Configdata["_flag_Filter_RC_CutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_AngleRate_CutOff = Configdata["_flag_Filter_AngleRate_CutOff"].get<double>();

	APMConfigTMP.FC._flag_Filter_PID_I_CutOff = Configdata["_flag_Filter_PID_I_CutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_PID_D_ST1_CutOff = Configdata["_flag_Filter_PID_D_ST1_CutOff"].get<double>();
	APMConfigTMP.FC._flag_Filter_PID_D_ST2_CutOff = Configdata["_flag_Filter_PID_D_ST2_CutOff"].get<double>();

	APMConfigTMP.FC._flag_GPS_Config_Beta = Configdata["_flag_GPS_Config_Beta"].get<double>();
	APMConfigTMP.FC._flag_Flow_Config_Beta = Configdata["_flag_Flow_Config_Beta"].get<double>();
	APMConfigTMP.FC._flag_Braking_Speed_Gain = Configdata["_flag_Braking_Speed_Gain"].get<double>();
	APMConfigTMP.FC._flag_Braking_AccelMax_Gain = Configdata["_flag_Braking_AccelMax_Gain"].get<double>();
	//=============================================================================//
	APSConfig.APMConfigSet(APMConfigTMP);
};

void CFG::ParseConfigFromJSONFile(std::string ConfigDir)
{
	std::ifstream config(ConfigDir);
	std::string content((std::istreambuf_iterator<char>(config)),
						(std::istreambuf_iterator<char>()));
	ParseConfigFromJSON(content);
};