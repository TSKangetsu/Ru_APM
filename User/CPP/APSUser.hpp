#include <functional>
#include <opencv2/opencv.hpp>

#define RTStatus_Real_Pitch 0
#define RTStatus_Real__Roll 1
#define RTStatus_Real_Speed_X 2
#define RTStatus_Speed_Y 3
#define RTStatus_Speed_Z 4
#define RTStatus_Movement_X 5
#define RTStatus_Movement_Y 6
#define RTStatus_Movement_Z 7
#define RTStatus_AltHoldTarget 8
#define RTStatus_FlowQuality 9
#define RTStatus_APMode 10

class APSCPPUser
{
public:
    void Setup();
    void Loop();

    void APSOpreator_FunctionRegs_RequestFrame(std::function<cv::Mat()> function);
    void APSOpreator_FunctionRegs_GetRTStatus(std::function<void(float *)> function);
    void APSOpreator_FunctionRegs_GetRCValues(std::function<void(float *)> function);
    void APSOpreator_FunctionRegs_SetServo(std::function<void(int, int, int)> function);
    void APSOpreator_FunctionRegs_RequestDISARM(std::function<void(bool)> function);
    void APSOpreator_FunctionRegs_SetUserSpeed(std::function<void(int, int, int)> function);
    void APSOpreator_FunctionRegs_SetUserPosition(std::function<void(int, int, int, bool)> function);

private:
    float Status[15];
    float RCValues[15];
    int clockTimer = 0;
    bool TakeoffFlag = false;
    bool LandingFlag = false;

    bool MissonASide = false;
    bool MissonBSide = false;
    bool MissonCSide = false;

    std::function<cv::Mat()> RequestFrame;
    std::function<void(bool)> RequestDISARM;
    std::function<void(float *)> GetRTStatus;
    std::function<void(float *)> GetRCValues;
    std::function<void(int, int, int)> SetServo;
    std::function<void(int, int, int)> SetUserSpeed;
    std::function<void(int, int, int, bool)> SetUserPosition;
};