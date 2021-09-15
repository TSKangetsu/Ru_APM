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
    bool VideoServerStop() { ProcessFlag = false; };

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

template <typename T>
class FrameBuffer : public std::queue<T>
{
public:
    unsigned int frameCount;
    FrameBuffer() : frameCount(0){};

    void pushFrame(const T &FrameBuffering)
    {
        std::lock_guard<std::mutex> lock(mutex);

        std::queue<T>::push(FrameBuffering);
        frameCount += 1;
        if (frameCount == 1)
        {
            timeDec.reset();
            timeDec.start();
        }
    }

    T getFrame()
    {
        std::lock_guard<std::mutex> lock(mutex);
        T lastestFrame = this->front();
        this->pop();
        return lastestFrame;
    }

    float getDecFPS()
    {
        timeDec.stop();
        double fps = frameCount / timeDec.getTimeSec();
        timeDec.start();
        return static_cast<float>(fps);
    }

    void clearBuffer()
    {
        std::lock_guard<std::mutex> lock(mutex);
        while (!this->empty())
            this->pop();
    }

private:
    cv::TickMeter timeDec;
    std::mutex mutex;
};

void VideoRecorderTest();