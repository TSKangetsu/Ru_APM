#pragma once
#ifdef DEBUG
#include <iostream>
#endif
#include <thread>
#include <opencv2/opencv.hpp>
#include "CameraDrive/Drive_V4L2Reader.hpp"
#include "VideoStreamDrive/Drive_RTMPPush.hpp"
#include "VideoStreamDrive/Drive_MJPEGServer.hpp"
#include "VideoStreamDrive/Drive_Socket.hpp"
#include "VisionAIDrive/Drive_OpenCVDN.hpp"

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
    std::string CameraDevice;
    PushMode VideoPushMode;
};

class VideoServer
{
public:
    int VideoServerReInit(VideoConfig &config);
    int VideoServerStop() { ProcessFlag = false; };

private:
    bool ProcessFlag;
    cv::Mat IMG;
    unsigned char *IMGTmp;
    V4L2Tools::V4l2Info CameraInfo;
    V4L2Tools::V4L2Drive *CameraDevice = nullptr;
    RTMPPusherOption RTMPInfo;
    RTMPPusher *RTMPServer = nullptr;
    AVFrame *RTMPFrame = nullptr;
    MJPEGServer *mJPEGServer = nullptr;
    std::thread PusherThread;
};
