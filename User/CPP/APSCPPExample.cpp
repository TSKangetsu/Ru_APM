#include "APSUser.hpp"

void APSCPPUser::Setup()
{
    std::cout << "setup call\n";
};

void APSCPPUser::Loop()
{
    float Status[15];
    float RCValues[15];
    GetRTStatus(Status);
    std::cout << "Real_Pitch:  " << Status[RTStatus_Real_Pitch] << "\n";
    std::cout << "Real_Roll:   " << Status[RTStatus_Real__Roll] << "\n";
    GetRCValues(RCValues);
    for (int i = 0; i < 10; i++)
    {
        std::cout << RCValues[i] << " ";
    }
    std::cout << "\n";
};