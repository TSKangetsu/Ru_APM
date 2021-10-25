#include <vector>
#include <thread>
#include <iostream>
#include <functional>
#ifdef MODULE_CV
#include <opencv2/opencv.hpp>
#endif

#define RTStatus_Real_Pitch 0
#define RTStatus_Real__Roll 1
#define RTStatus_Speed_X 2
#define RTStatus_Speed_Y 3
#define RTStatus_Speed_Z 4
#define RTStatus_Movement_X 5
#define RTStatus_Movement_Y 6
#define RTStatus_Movement_Z 7
#define RTStatus_AltHoldTarget 8
#define RTStatus_FlowQuality 9
#define RTStatus_APMode 10
#define RTStatus_IsARM 11
#define RTStatus_RunTime 12
#define RTStatus_RunTimeError 13

//This is User Custcom emaxple only
int IsARM = 1;
float RTStatus[14];
int ChannelARMData = 1000;
#ifdef MODULE_CV
cv::VideoCapture MainCap;
#endif
std::thread IOStream;
//=================================//

namespace UserAPICPP
{
    std::function<void(bool)> RequestDISARM;
    std::function<void(float *)> GetRTStatus;
    std::function<void(float *)> GetRCValues;
    std::function<void(int, int, int)> SetServo;
    std::function<void(int, int, int)> SetUserSpeed;
#ifdef MODULE_CV
    std::function<void(cv::Mat &, int)> RequestFrame;
#endif
    std::function<void(int *, bool)> FakeRCImplment;
    std::function<void(int, int, int, bool)> SetUserPosition;

    void UserSetup();
    void UserLoop();
}
