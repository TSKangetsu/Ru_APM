#ifndef CONFIG_H
#define CONFIG_H
#endif
#pragma once

#include <iostream>
#include "../ToolsInclude/M_Tools.hpp"
#include "../_thirdparty/json/single_include/nlohmann/json.hpp"

namespace JsonConfig
{
class CVConfig
{
private:
	nlohmann::json CVexample;
};

class SensorsInfo
{
public:
	SensorsInfo(std::string filename, std::string &configDump);
	~SensorsInfo();

private:
	nlohmann::json example = R"(
		{
			"SensorConfigure":
			[
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