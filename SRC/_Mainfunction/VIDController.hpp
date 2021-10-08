#pragma once
#include <map>
#include <opencv2/opencv.hpp>

#include "UORBMessage.hpp"
#include "../_Excutable/LogPublicator.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../_WIFIBroadcast/WIFICastDriver.hpp"
#include "../_VisionBase/CameraDrive/Drive_V4L2Reader.hpp"

using VIDS = RuAPSSys::ConfigCLA;

#define EMAP(Variable) (#Variable)

enum VideoFormat
{
    YUYV,
    BGR3,
    H264,
    H265,
    MJPEG,
};

inline static const std::map<std::string, VideoFormat> VideoFormat_s =
    {
        {EMAP(YUYV), YUYV},
        {EMAP(BGR3), BGR3},
        {EMAP(H264), H264},
        {EMAP(H265), H265},
        {EMAP(MJPEG), MJPEG},
};

inline static const std::map<std::string, unsigned int> V4L2Format_s =
    {
        {EMAP(YUYV), V4L2_PIX_FMT_YUYV},
        {EMAP(BGR3), V4L2_PIX_FMT_BGR24},
        {EMAP(H264), V4L2_PIX_FMT_H264},
        {EMAP(H265), V4L2_PIX_FMT_HEVC},
        {EMAP(MJPEG), V4L2_PIX_FMT_MJPEG},
};

enum VideoDriver
{
    V4L2,
    OPENCV,
    FFMPEG,
    GSTREAM,
};

inline static const std::map<std::string, VideoDriver> VideoDriver_s =
    {
        {EMAP(V4L2), V4L2},
        {EMAP(OPENCV), OPENCV},
        {EMAP(FFMPEG), FFMPEG},
        {EMAP(GSTREAM), GSTREAM},
};

class VIDController_t
{
public:
    VIDController_t();
    ~VIDController_t();

private:
    void VideoISLoader();
    std::vector<std::unique_ptr<FlowThread>> VideoISThread;
    std::vector<std::unique_ptr<cv::VideoCapture>> CVCAMDriver;
    std::vector<std::unique_ptr<V4L2Tools::V4L2Drive>> V4L2Driver;
};

VIDController_t::VIDController_t()
{
    // Step 1. Sync and Setup config

    for (size_t i = 0; i < VIDS::VideoConfig.size(); i++)
    {
        if (VIDS::VideoConfig[i].enable)
        {
            switch (VideoDriver_s.at(VIDS::VideoConfig[i].DeviceDriver))
            {

            case VideoDriver::V4L2:
            {
                std::unique_ptr<V4L2Tools::V4L2Drive> V4L2P;
                try
                {
                    RuAPSSys::UORBMessage::StreamStatus.VideoIFlowRaw.push_back(std::make_tuple(nullptr, VIDS::VideoConfig[i], -1));
                    V4L2P.reset(
                        new V4L2Tools::V4L2Drive(
                            VIDS::VideoConfig[i].DevicePATH,
                            {.ImgWidth = VIDS::VideoConfig[i].DeviceWidth,
                             .ImgHeight = VIDS::VideoConfig[i].DeviceHeight,
                             .FrameRate = VIDS::VideoConfig[i].DeviceFPS,
                             .FrameBuffer = 2,
                             .Is_fastMode = false,
                             .Is_AutoSize = (VIDS::VideoConfig[i].DeviceWidth < 0),
                             .PixFormat = V4L2Format_s.at(VIDS::VideoConfig[i].DeviceIFormat)}));
                    std::get<2>(RuAPSSys::UORBMessage::StreamStatus.VideoIFlowRaw[i]) = V4L2P->V4L2InfoGet().ImgDataSize;
                    V4L2Driver.push_back(std::move(V4L2P));
                }
                catch (int &e)
                {
                    RuAPSSys::UORBMessage::SystemStatus.SystemMessage.push(
                        _VID << "V4L2 Init Error Skip:" << i << " error:" << e << "\n");
                }
            }
            break;
            case VideoDriver::OPENCV:
            {
                std::unique_ptr<cv::VideoCapture> CVP;
                try
                {
                    cv::Mat Tmp;
                    RuAPSSys::UORBMessage::StreamStatus.VideoICVRaw.push_back(std::make_tuple(Tmp, VIDS::VideoConfig[i]));
                    CVP.reset(new cv::VideoCapture());
                    if (!CVP->open(VIDS::VideoConfig[i].DevicePATH))
                        throw - 1;
                    CVP->set(cv::CAP_PROP_FRAME_WIDTH, VIDS::VideoConfig[i].DeviceWidth);
                    CVP->set(cv::CAP_PROP_FRAME_HEIGHT, VIDS::VideoConfig[i].DeviceWidth);
                    CVP->set(cv::CAP_PROP_FPS, VIDS::VideoConfig[i].DeviceFPS);
                    CVP->set(cv::CAP_PROP_FOURCC, V4L2Format_s.at(VIDS::VideoConfig[i].DeviceIFormat));
                    CVCAMDriver.push_back(std::move(CVP));
                }
                catch (int &e)
                {
                    RuAPSSys::UORBMessage::SystemStatus.SystemMessage.push(
                        _VID << "cvVID Init Error Skip:" << i << " error:" << e << "\n");
                }
            }
            break;

            default:
                break;
            };
        }
    }

    VideoISLoader();
};

void VIDController_t::VideoISLoader()
{
    for (size_t i = 0; i < V4L2Driver.size(); i++)
    {
        std::unique_ptr<FlowThread> VideoIThread;
        VideoIThread.reset(new FlowThread(
            [&, s = i]() {
                std::get<0>(RuAPSSys::UORBMessage::StreamStatus.VideoIFlowRaw[s]) =
                    V4L2Driver[s]->V4L2Read();
            },
            VIDS::VideoConfig[i].DeviceFPS));

        VideoISThread.push_back(std::move(VideoIThread));
    }

    for (size_t i = 0; i < CVCAMDriver.size(); i++)
    {
        std::unique_ptr<FlowThread> VideoIThread;
        VideoIThread.reset(new FlowThread(
            [&, s = i]() {
                CVCAMDriver[s]->read(std::get<0>(RuAPSSys::UORBMessage::StreamStatus.VideoICVRaw[s]));
            },
            VIDS::VideoConfig[i].DeviceFPS));

        VideoISThread.push_back(std::move(VideoIThread));
    }
};

VIDController_t::~VIDController_t()
{
    for (size_t i = 0; i < VideoISThread.size(); i++)
    {
        VideoISThread[i]->FlowStopAndWait();
    }

    for (size_t i = 0; i < V4L2Driver.size(); i++)
    {
        V4L2Driver[i].reset();
    }

    for (size_t i = 0; i < CVCAMDriver.size(); i++)
    {
        CVCAMDriver[i].reset();
    }
};