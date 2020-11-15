#pragma once
#ifdef DEBUG
#include <iostream>
#endif
#include <thread>
#include <opencv2/opencv.hpp>
#include "../_Excutable/Drive_Socket.hpp"
#include "CameraDrive/Drive_V4L2Reader.hpp"
#include "VideoStreamDrive/Drive_RTMPPush.hpp"
#include "VideoStreamDrive/Drive_MJPEGServer.hpp"
#include "VisionAIDrive/Drive_OpenCVDN.hpp"
#include "XthermCameraDrive/XthermSDK.hpp"

enum PushMode
{
    RTMP,
    MJPEG,
    X11Display,
};

struct VideoConfig
{
    int Width;
    int Height;
    int SubWidth;
    int SubHeight;
    std::string CameraDevice;
    std::string CameraDeviceSub;
    PushMode VideoPushMode;
};

class VideoServer
{
public:
    int VideoServerReInit(VideoConfig &config);
    int VideoServerStop() { ProcessFlag = false; };

private:
    bool ProcessFlag;
    bool IsCamInited = false;
    float *TempData;
    cv::Mat IMG;
    cv::Mat SubIMG;
    cv::Mat SubIMGFinal;
    cv::Mat FinalIMG;
    unsigned char *IMGTmp;
    unsigned char *SubIMGTmp;
    unsigned char *SubIMGTmpRGB24;
    V4L2Tools::V4l2Info CameraInfo;
    V4L2Tools::V4l2Info CameraInfoSub;
    V4L2Tools::V4L2Drive *CameraDevice = nullptr;
    V4L2Tools::V4L2Drive *CameraDeviceSub = nullptr;
    RTMPPusherOption RTMPInfo;
    RTMPPusher *RTMPServer = nullptr;
    AVFrame *RTMPFrame = nullptr;
    MJPEGServer *mJPEGServer = nullptr;
    std::thread PusherThread;
    std::thread VideoSub;
    CVInferEngine *InferEngine;
    CVInferConfig InferConfig;
    XthermSDKOption xtopt;
    std::string RoundNumber(float result)
    {
        std::ostringstream out;
        out << std::setprecision(1) << std::fixed << std::showpoint << result;
        std::string RoundResult = out.str();
        return RoundResult;
    }
};
