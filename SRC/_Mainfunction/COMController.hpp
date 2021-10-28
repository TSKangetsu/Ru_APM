#pragma once
#include <string.h>

#include "../_Excutable/FlowController.hpp"
#include "../_WIFIBroadcast/WIFICastDriver.hpp"

#ifdef MODULE_FFMPEG
#include "../_Thirdparty/FFMPEG/FFMPEGCodec.hpp"
#endif

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

    int BroadCastDataCount = 0;
    std::unique_ptr<WIFICastDriver> Injector;
    std::unique_ptr<FlowThread> NormalThread;
    std::unique_ptr<FlowThread> BroadcastThread;
#ifdef MODULE_FFMPEG
    std::queue<FFMPEGTools::AVData> EncoderQueue;
    std::unique_ptr<FFMPEGTools::FFMPEGCodec> Encoder;
#endif
};

COMController_t::COMController_t()
{
    if (!(std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat == "H264" ||
          std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat == "H265"))
        Encoder.reset(new FFMPEGTools::FFMPEGCodec({
            .IOWidth = SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceWidth,
            .IOHeight = SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceHeight,
            .OBuffer = 4,
            .OFrameRate = SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceFPS,
            .OBitRate = 280000,
            .CodecProfile = "high444",
            .OutputFormat = AV_CODEC_ID_H264,
            .TargetFormat = AV_PIX_FMT_YUYV422,
        }));
    // Step 1:
    if (SYSC::CommonConfig.COM_BroadCastEnable)
    {
        Injector.reset(new WIFICastDriver(SYSC::CommonConfig.BroadcastInterfaces));

        if (SYSU::StreamStatus.VideoIFlowRaw.size() > 0)
        {
            BroadcastThread.reset(new FlowThread(
                [&]() {
                    // Step 0. Target Video data
                    V4L2Tools::V4l2Data data;
                    size_t InjectVSize = 0;
                    uint8_t *InjectVTarget = nullptr;
                    // Step 1. Read From uorb
                    if (std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).frameCount > 0)
                        data = std::get<FrameBuffer<V4L2Tools::V4l2Data>>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).peekFrame();
                    // Step 2. Transcodec or not, deal with VID data
                    if (data.size > 0)
                    {
                        if (std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat == "H264" ||
                            std::get<SYSC::VideoSettings>(SYSU::StreamStatus.VideoIFlowRaw[SYSC::CommonConfig.COM_CastFrameIndex]).DeviceIFormat == "H265")
                        {
                            InjectVSize = data.size;
                            InjectVTarget = data.data;
                        }
                        else
                        {
                            if (EncoderQueue.size() >= 2)
                                EncoderQueue.pop();

                            Encoder->pushFrame(data.data, data.size, data.bytesperline);
                            Encoder->getFrame(EncoderQueue);
                            //
                            InjectVSize = EncoderQueue.front().size;
                            InjectVTarget = EncoderQueue.front().data;
                        }
                        // ...
                        // Step N. Inject data
                        Injector->WIFICastInject(InjectVTarget, InjectVSize, 0, BroadCastType::VideoStream, 0, SYSC::CommonConfig.COM_CastFrameIndex);
                        // Step N + 1. Inject img info.
                        BroadCastDataCount++;
                        if (BroadCastDataCount >= (float)SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceFPS)
                        {
                            BroadCastDataCount = 0;
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
                        }
                    }
                    COMBoradCastDataInject();
                },
                (float)SYSC::VideoConfig[SYSC::CommonConfig.COM_CastFrameIndex].DeviceFPS));
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

    if (BroadcastThread != nullptr)
        BroadcastThread->FlowStopAndWait();

    if (Injector != nullptr)
        Injector.reset();
}