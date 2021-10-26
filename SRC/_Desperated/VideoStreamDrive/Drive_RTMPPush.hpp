#pragma once
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

struct RTMPPusherOption
{
    int _flag_Width = 1280;
    int _flag_Height = 720;
    int _flag_Bitrate = 300000;
    int _flag_FPS = 15;
    int _flag_BufferSize = 12;
    std::string _flag_RTMPStreamURL = "rtmp://0.0.0.0/live";
    std::string _flag_CodeCProfile = "high444";
    std::string _flag_StreamFormat = "flv";
};

class RTMPPusher
{
public:
    RTMPPusher(RTMPPusherOption option);
    bool RTMPEncodecerInit();
    AVFrame *CVMattToAvframe(cv::Mat *image, AVFrame *frame);
    bool RTMPSender(AVFrame *TargetFrame);
    ~RTMPPusher();

private:
    int _flag_Error;
    RTMPPusherOption _flag_Option;
    AVRational _flag_Codec_FPS;
    SwsContext *MainSws;
    AVFrame *SBUFFFrame;
    AVPacket _TmpPacket = {0};
    AVCodec *BaseCodec = nullptr;
    AVStream *BaseFormatStream = nullptr;
    AVFormatContext *BaseFormatContext = nullptr;
    AVCodecContext *BaseCodecContext = nullptr;
};