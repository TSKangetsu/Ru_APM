#include "M_Sensors.hpp"

Sensors::SensorsInfo::SensorsInfo()
{
}

int Sensors::SensorsInfo::ConfigureCheck()
{
}

bool Sensors::SensorsInfo::Createconfigure(std::string filename)
{
    std::ofstream config;
    config.open("./sensors.json");
    nlohmann::json example = {
            {"deviceName" , "BH1750"},
            {"I2CAddress" ,     0x23},
            {"startFlag" , {
                    {"addr01" , "value"},
                    {"addr02" , "value"},
                },
            },
            {"dataAddr",
                {
                    "dataAddr01",
                    "dataAddr02",
                    "dataaddr03",
                }
            }
    };
    example.object.dump();
}

std::string *Sensors::SensorsInfo::ReadConfigure()
{
}