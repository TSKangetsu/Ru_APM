#pragma once
#include <map>
#include <opencv2/opencv.hpp>

#include "UORBMessage.hpp"
#include "../_Excutable/ThreadBuffer.hpp"
#include "../_Excutable/LogPublicator.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../_WIFIBroadcast/WIFICastDriver.hpp"
#include "../_VisionBase/CameraDrive/Drive_V4L2Reader.hpp"

using SYSC = RuAPSSys::ConfigCLA;
using SYSU = RuAPSSys::UORBMessage;

#define EMAP(Variable) (#Variable)
#define MAXV4LBUF 1
#define MAXBUFFER 3

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

    for (size_t i = 0; i < SYSC::VideoConfig.size(); i++)
    {
        if (SYSC::VideoConfig[i].enable)
        {
            switch (VideoDriver_s.at(SYSC::VideoConfig[i].DeviceDriver))
            {

            case VideoDriver::V4L2:
            {
                std::unique_ptr<V4L2Tools::V4L2Drive> V4L2P;
                try
                {
                    V4L2P.reset(
                        new V4L2Tools::V4L2Drive(
                            SYSC::VideoConfig[i].DevicePATH,
                            {.ImgWidth = SYSC::VideoConfig[i].DeviceWidth,
                             .ImgHeight = SYSC::VideoConfig[i].DeviceHeight,
                             .FrameRate = SYSC::VideoConfig[i].DeviceFPS,
                             .FrameBuffer = MAXV4LBUF,
                             .Is_AutoSize = (SYSC::VideoConfig[i].DeviceWidth < 0),
                             .PixFormat = V4L2Format_s.at(SYSC::VideoConfig[i].DeviceIFormat)}));

                    FrameBuffer<V4L2Tools::V4l2Data> Data;
                    SYSU::StreamStatus.VideoIFlowRaw.push_back(
                        std::make_tuple(std::move(Data), SYSC::VideoConfig[i]));

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
                    CVP.reset(new cv::VideoCapture());
                    if (!CVP->open(SYSC::VideoConfig[i].DevicePATH))
                        throw - 1;
                    CVP->set(cv::CAP_PROP_FRAME_WIDTH, SYSC::VideoConfig[i].DeviceWidth);
                    CVP->set(cv::CAP_PROP_FRAME_HEIGHT, SYSC::VideoConfig[i].DeviceWidth);
                    CVP->set(cv::CAP_PROP_FPS, SYSC::VideoConfig[i].DeviceFPS);
                    CVP->set(cv::CAP_PROP_FOURCC, V4L2Format_s.at(SYSC::VideoConfig[i].DeviceIFormat));

                    FrameBuffer<cv::Mat> Data;
                    SYSU::StreamStatus.VideoICVRaw.push_back(
                        std::make_tuple(std::move(Data), SYSC::VideoConfig[i]));

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
                if (std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[s]).frameCount > MAXBUFFER)
                    std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[s]).getFrame();

                V4L2Tools::V4l2Data data;
                V4L2Driver[s]->V4L2Read(data);
                std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[s]).pushFrame(data);
            }));

        VideoISThread.push_back(std::move(VideoIThread));
    }

    for (size_t i = 0; i < CVCAMDriver.size(); i++)
    {
        std::unique_ptr<FlowThread> VideoIThread;
        VideoIThread.reset(new FlowThread(
            [&, s = i]() {
                if (std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[s]).frameCount > MAXBUFFER)
                    std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[s]).getFrame();

                cv::Mat data;
                CVCAMDriver[s]->read(data);
                std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[s]).pushFrame(data);
            }));

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