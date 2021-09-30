#pragma once
#include <map>
#include <opencv2/opencv.hpp>

#include "UORBMessage.hpp"
#include "../_Excutable/LogPublicator.hpp"
#include "../_Excutable/LogPublicator.hpp"
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

static const std::map<std::string, VideoFormat> VideoFormat_s =
    {
        {EMAP(YUYV), YUYV},
        {EMAP(BGR3), BGR3},
        {EMAP(H264), H264},
        {EMAP(H265), H265},
        {EMAP(MJPEG), MJPEG},
};

static const std::map<std::string, unsigned int> V4L2Format_s =
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

static const std::map<std::string, VideoDriver> VideoDriver_s =
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

    void VIDControllerReload();

private:
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
            }
            break;

            default:
                break;
            };
        }
    }
};

VIDController_t::~VIDController_t()
{
    for (size_t i = 0; i < V4L2Driver.size(); i++)
    {
        V4L2Driver[i]->~V4L2Drive();
    }
};