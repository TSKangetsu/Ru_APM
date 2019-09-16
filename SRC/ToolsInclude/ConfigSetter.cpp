#include "M_Tools.hpp"

bool Base::ConfigSetter::CreateConfig(std::string fileName, std::string data)
{
	std::ofstream config;
	config.open(fileName.c_str());
	config << data.c_str();
	config.close();
	return true;
}

bool Base::ConfigSetter::ReadConfig(std::string fileName, std::string& data)
{
	std::ifstream ifs(fileName);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	data = content;
	return true;
}