#pragma once
#include <string.h>
#include <opencv2/opencv.hpp>

#include "VIDController.hpp"
#include "../_Excutable/FlowController.hpp"
#include "../_WIFIBroadcast/WIFICastDriver.hpp"
#include "../_MessageController/MessageController.hpp"

using namespace WIFIBroadCast;
using SYSU = RuAPSSys::UORBMessage;
using SYSC = RuAPSSys::ConfigCLA;

class COMController_t
{
public:
    COMController_t();
    ~COMController_t();

private:
    void COMBoradCastDataInject();
    std::unique_ptr<WIFICastDriver> Injector;
    std::unique_ptr<FlowThread> NormalThread;
    std::unique_ptr<FlowThread> BoradcastThread;
};

COMController_t::COMController_t()
{
    // Step 1:
    if (SYSC::CommonConfig.COM_BroadCastEnable)
    {
        Injector.reset(new WIFICastDriver(SYSC::CommonConfig.BroadcastInterfaces));

        if (SYSU::StreamStatus.VideoIFlowRaw.size() > 0 || SYSU::StreamStatus.VideoICVRaw.size() > 0)
        {
            switch (VideoDriver_s.at(SYSC::CommonConfig.COM_CastFrameType))
            {
            case VideoDriver::V4L2:
                BoradcastThread.reset(new FlowThread(
                    [&]() {
                        // Step 0. Target Video data
                        V4L2Tools::V4l2Data data;
                        size_t InjectVSize = 0;
                        uint8_t *InjectVTarget = nullptr;
                        // Step 1. Read From uorb
                        if (std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).frameCount > 2)
                            data = std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).getFrame();
                        // Step 2. Transcodec or not, deal with VID data
                        if (std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat ==
                            std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceOFormat)
                        {
                            InjectVSize = data.size;
                            InjectVTarget = data.data;
                        }
                        else
                        {
                        }
                        // ...
                        // Step N. Inject data
                        Injector->WIFICastInject(InjectVTarget, InjectVSize, 0, BroadCastType::VideoStream, 0, SYSC::CommonConfig.COM_CastFrameIndex);
                        // Step N + 1. Inject img info.
                        uint8_t ImgInfo[] = {
                            (uint8_t)(SYSC::CommonConfig.COM_CastFrameIndex),
                            (uint8_t)(data.maxsize >> 24),
                            (uint8_t)(data.maxsize >> 16),
                            (uint8_t)(data.maxsize >> 8),
                            (uint8_t)(data.maxsize),
                            (uint8_t)(std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceWidth >> 8),
                            (uint8_t)(std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceWidth),
                            (uint8_t)(std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceHeight >> 8),
                            (uint8_t)(std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceHeight),
                        };
                        Injector->WIFICastInject(ImgInfo, sizeof(ImgInfo), 0, BroadCastType::DataStream, 0, 0xf);
                        COMBoradCastDataInject();
                    },
                    (float)SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceFPS));
                break;

            case VideoDriver::OPENCV:
                BoradcastThread.reset(new FlowThread(
                    [&]() {
                        // Step 0. Target Video data
                        cv::Mat data;
                        size_t InjectVSize = 0;
                        uint8_t *InjectVTarget = nullptr;
                        // Step 1. Read From uorb
                        if (std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[SYSC::CommonConfig.COM_CastFrameIndex]).frameCount > 2)
                            data = std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[SYSC::CommonConfig.COM_CastFrameIndex]).getFrame();
                        // Step 2. Transcodec or not, deal with VID data
                        if (std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat ==
                            std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceOFormat)
                        {
                            InjectVSize = data.total() * data.elemSize();
                            InjectVTarget = data.data;
                        }
                        else
                        {
                        }
                        // ...
                        // Step N. Inject data
                        Injector->WIFICastInject(InjectVTarget, InjectVSize, 0, BroadCastType::VideoStream, 0, SYSC::CommonConfig.COM_CastFrameIndex);
                        COMBoradCastDataInject();
                    },
                    (float)SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceFPS));

            default:
                break;
            }
        }
    }
}

void COMController_t::COMBoradCastDataInject()
{
}

COMController_t::~COMController_t()
{
    if (NormalThread != nullptr)
        NormalThread->FlowStopAndWait();

    if (BoradcastThread != nullptr)
        BoradcastThread->FlowStopAndWait();

    if (Injector != nullptr)
        Injector.reset();
}