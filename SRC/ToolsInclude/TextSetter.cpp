#include "M_Tools.hpp"

bool Base::TextSetter::CreateConfig(std::string fileName, std::string configDump)
{
	std::ofstream config;
	config.open(fileName);
	config << configDump;
	config.close();
	return true;
}

bool Base::TextSetter::ReadConfig(std::string fileName, std::string &configDump)
{
	std::ifstream config(fileName);
	std::string line;
	config >> configDump;
	config.close();
	return true;
}