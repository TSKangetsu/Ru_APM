#pragma once
#include "../_thirdparty/json/single_include/nlohmann/json.hpp"
#include "../ToolsInclude/M_Tools.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

namespace JsonConfig
{
class CVConfig
{
public:
	CVConfig(std::string filename);

private:
	std::string dataTmp;
	const std::string cvConfigName = "./CVConfig.json";
	nlohmann::json cvExample = R"(
		{
			"CameraConfig":{
				"Camera_FrameBuff": 1,
				"Camera_Height": 300,
				"Camera_Width": 300
			},
			"DnnConfig":{
				"Blob_Scalar":[
					100,
					100,
					100
				],
				"Blob_Size":[
					300,
					300
				], 
				"Confidence": 0.4,
				"Dnn_Model": 1,
				"PreferableBackend": 0,
				"PreferableTarget": 0,
				"ModelDir":{
					"args1": "none",
					"args2": "none"
				}
			}
		}
		)"_json;
};

class SensorsInfo
{
public:
	SensorsInfo(std::string filename);

private:
	std::string dataTmp;
	const std::string sensorsConfigName = "./Sensors.json";
	nlohmann::json sensorsExample = R"(
		{
			"SensorConfigure":[
				{
					"Device": "BH1750VI",
					"DeviceKinds": "LIGHTSENSORS",
					"I2CAddress":"0x23",
					"StartFlag":{
						"addr01": "value",
						"addr02": "value",
						"addr02": "value"
					},
					"VerDataAddress": [
						"dataaddr01",
						"dataaddr02",
						"dataaddr03"
					]
				}
			]
		}
	)"_json;
};
} // namespace JsonConfig