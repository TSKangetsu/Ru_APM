#include "M_Tools.hpp"

bool Base::ConfigSetter::CreateConfig(std::string fileName, std::string data)
{
    std::ofstream config;
    config.open(fileName);
    config << data;
    config.close();
}

bool Base::ConfigSetter::ReadConfig(std::string fileName, std::string &data)
{
    std::ifstream config(fileName);
    config >> data;
    config.close();
}