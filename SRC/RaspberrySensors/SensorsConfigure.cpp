#include "M_Sensors.hpp"

Sensors::SensorsInfo::SensorsInfo(std::string fileName)
{
	configName = fileName;
	if (access(configName.c_str(), F_OK) != -1)
	{
		if (access(configName.c_str(), R_OK) != -1)
		{
			std::cout << "\033[33m[Sensors] config file found , reading ........\033[0m\n";
			ReadConfig(fileName);
		}
		else
		{
			std::cout << "\033[31m[Sensors] config file false , please check user func\033[0m\n";
		}
	}
	else
	{
		std::cout << "\033[33m[Sensors] config file no found , will create a example\033[0m\n";
		CreateConfig(configName);
	}
}

Sensors::SensorsInfo::~SensorsInfo()
{

}