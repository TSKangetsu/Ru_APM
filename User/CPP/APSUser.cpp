#include "APSUser.hpp"

void APSCPPUser::APSOpreator_FunctionRegs_RequestFrame(std::function<cv::Mat()> function)
{
    RequestFrame = function;
};
void APSCPPUser::APSOpreator_FunctionRegs_GetRTStatus(std::function<void(float *)> function)
{
    GetRTStatus = function;
};

void APSCPPUser::APSOpreator_FunctionRegs_GetRCValues(std::function<void(float *)> function)
{
    GetRCValues = function;
};

void APSCPPUser::APSOpreator_FunctionRegs_SetServo(std::function<void(int, int, int)> function)
{
    SetServo = function;
};

void APSCPPUser::APSOpreator_FunctionRegs_RequestDISARM(std::function<void(bool)> function)
{
    RequestDISARM = function;
}

void APSCPPUser::APSOpreator_FunctionRegs_SetUserSpeed(std::function<void(int, int, int)> function)
{
    SetUserSpeed = function;
}

void APSCPPUser::APSOpreator_FunctionRegs_SetUserPosition(std::function<void(int, int, int, bool)> function)
{
    SetUserPosition = function;
}