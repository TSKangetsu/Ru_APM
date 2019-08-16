#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include "../_thirdparty/json/single_include/nlohmann/json.hpp"

namespace Sensors
{
	class SensorsInfo
	{
	public:
		nlohmann::json configJson;
		SensorsInfo(std::string filename);
		~SensorsInfo();
	private:
		std::string configName;
		std::string configRead;
		//根据json序列化标准,nlohmann的作者不愿意手动集成按插入顺序集成的功能,我不想再调他写的其他库,就这样
		nlohmann::json example = R"({
			"Configure":[
			{
				"Device": "BH1750VI",
				"DeviceKinds": "LIGHTSENSORS",
				"I2CAddress":"0x23",
				"StartFlag":{
					"addr01": "value",
					"addr02": "value",
					"addr02": "value"
				},
				"VdataAddress": [
					"dataaddr01",
					"dataaddr02",
					"dataaddr03"
				]
			}
		]})"_json;
		bool CreateConfig(std::string fileName)
		{
			std::ofstream config;
			config.open(fileName);
			config << example.dump(4);
			config.close();
			return true;
		}
		bool ReadConfig(std::string fileName)
		{
			std::ifstream config(fileName);
			std::string line;
			config >> configJson;
			config.close();
		}
	};


	class SeLight
	{
	public:
		SeLight(int I2C_ADDR);

	private:
		int DEVICEADDR;
	};
}
