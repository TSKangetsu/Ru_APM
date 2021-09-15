#pragma once
#include <queue>
#include <string>

namespace RuAPSSys
{
	enum APMMessageDataType
	{
		AT___Int,
		AT__Bool,
		AT_Float,
	};
	//
	enum APMMessageSheet
	{
		_uORB_Accel_X,
		_uORB_Accel_Y,
		_uORB_Accel_Z,
		_uORB_Real__Roll,
		_uORB_Real_Pitch,
		_uORB_Real___Yaw,
		_uORB_Gyro__Roll,
		_uORB_Gyro_Pitch,
		_uORB_Gyro___Yaw,
		_uORB_Leveling__Roll,
		_uORB_Leveling_Pitch,
		_uORB_Leveling___Yaw,
		_uORB_Alt_Pressure,
		_uORB_Accel_VIBE_X,
		_uORB_Accel_VIBE_Y,
		_uORB_Accel_VIBE_Z,
		_flag_Controller_AutopilotBank,
		_flag_Controller_ARM,
	};
	//
	struct APMMessageType
	{
		std::string Name;
		APMMessageDataType DataType;
		int Data;
	};
	//----------------------------------------------------------------//
	struct APSMessageType
	{
		std::string Message;
		std::string MessageUser;
		uint32_t MessageCode;
	};
	//----------------------------------------------------------------//
	class UORBMessage
	{
	public:
		// APSMessage is Report 100HZ By CONController
		struct APSMessage_t
		{
			std::queue<APSMessageType> APSMessageGroup;
			bool IsAPSMessageUpdate = false;
		} APSMessage;
		// APMMessage is Report 250HZ By APMController
		struct APMMessage_t
		{
			APMMessageType APMMessageGroup[35] =
				{
					{"_uORB_Accel_X", APMMessageDataType::AT_Float, 0},
					{"_uORB_Accel_Y", APMMessageDataType::AT_Float, 0},
					{"_uORB_Accel_Z", APMMessageDataType::AT_Float, 0},
					{"_uORB_Real__Roll", APMMessageDataType::AT_Float, 0},
					{"_uORB_Real_Pitch", APMMessageDataType::AT_Float, 0},
					{"_uORB_Real___Yaw", APMMessageDataType::AT_Float, 0},
					{"_uORB_Gyro__Roll", APMMessageDataType::AT_Float, 0},
					{"_uORB_Gyro_Pitch", APMMessageDataType::AT_Float, 0},
					{"_uORB_Gyro___Yaw", APMMessageDataType::AT_Float, 0},
					{"_uORB_Leveling__Roll", APMMessageDataType::AT_Float, 0},
					{"_uORB_Leveling_Pitch", APMMessageDataType::AT_Float, 0},
					{"_uORB_Leveling___Yaw", APMMessageDataType::AT_Float, 0},
					{"_uORB_Alt_Pressure", APMMessageDataType::AT___Int, 0},
					{"_uORB_Accel_VIBE_X", APMMessageDataType::AT___Int, 0},
					{"_uORB_Accel_VIBE_Y", APMMessageDataType::AT___Int, 0},
					{"_uORB_Accel_VIBE_Z", APMMessageDataType::AT___Int, 0},
					{"_flag_Controller_AutopilotBank", APMMessageDataType::AT___Int, 0},
					{"_flag_Controller_ARM", APMMessageDataType::AT__Bool, 0},
			};
			bool IsAPMMessageUpdate = false;
		} APMMessage;
	};
}
