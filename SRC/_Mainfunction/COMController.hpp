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
                        // If Input stream is the same as output stream, do nothing and inject to air
                        if (std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat ==
                            std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceOFormat)
                        {
                            if (std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).frameCount > 2)
                            {
                                V4L2Tools::V4l2Data data = std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).getFrame();

                                Injector->WIFICastInject(data.data, data.size, 0, BroadCastType::VideoStream, 0);
                            }
                        };

                        COMBoradCastDataInject();
                    },
                    (float)std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceFPS));
                break;

            case VideoDriver::OPENCV:
                BoradcastThread.reset(new FlowThread(
                    [&]() {
                        if (std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[SYSC::CommonConfig.COM_CastFrameIndex]).frameCount > 2)
                        {
                            cv::Mat data = std::get<FrameBuffer<cv::Mat>>(SYSU::StreamStatus.VideoICVRaw[SYSC::CommonConfig.COM_CastFrameIndex]).getFrame();

                            Injector->WIFICastInject(data.data, (data.total() * data.elemSize()), 0, BroadCastType::VideoStream, 0);
                        }

                        COMBoradCastDataInject();
                    },
                    (float)std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoICVRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceFPS));
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