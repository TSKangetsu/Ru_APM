#include "M_Config.hpp"

JsonConfig::SensorsInfo::SensorsInfo(std::string fileName, std::string &configDump)
{
	if (access(fileName.c_str(), F_OK) != -1)
	{
		if (access(fileName.c_str(), R_OK) != -1)
		{
			std::cout << "\033[33m[Sensors] config file found , reading ........\033[0m\n";
			Base::TextSetter::ReadConfig(fileName, configDump);
		}
		else
		{
			std::cout << "\033[31m[Sensors] config file false , please check user func\033[0m\n";
		}
	}
	else
	{
		std::cout << "\033[33m[Sensors] config file no found , will create a example\033[0m\n";
		Base::TextSetter::CreateConfig(fileName, example.dump(4));
	}
}

JsonConfig::SensorsInfo::~SensorsInfo()
{
}