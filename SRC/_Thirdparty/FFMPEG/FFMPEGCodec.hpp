#pragma once
#include <queue>
#include <tuple>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
namespace FFMPEGTools
{
    struct AVData
    {
        int width;
        int height;
        unsigned int size;
        unsigned char *data;
        //
        AVData() : width(0), height(0), size(0), data(nullptr){};
        AVData(int width, int height, unsigned int size)
        {
            this->width = width;
            this->height = height;
            this->size = size;
            this->data = new unsigned char[this->size];
        };
        AVData &operator=(const AVData &DataCpy)
        {
            width = DataCpy.width;
            height = DataCpy.height;
            size = DataCpy.size;
            data = new unsigned char[size];
            std::copy(DataCpy.data, DataCpy.data + size, this->data);
            return *this;
        };
        AVData(const AVData &DataCpy)
        {
            width = DataCpy.width;
            height = DataCpy.height;
            size = DataCpy.size;
            data = new unsigned char[size];
            std::copy(DataCpy.data, DataCpy.data + size, this->data);
        };
        ~AVData()
        {
            if (data != nullptr)
                delete[] data;
        };
    };

    struct FFMPEGOption
    {
        int IOWidth;
        int IOHeight;
        int OBuffer;
        int OFrameRate;
        int OBitRate;
        const char *CodecProfile;

        AVCodecID OutputFormat;
        AVPixelFormat TargetFormat;
    };

    class FFMPEGCodec
    {
    public:
        FFMPEGCodec(FFMPEGOption);
        void pushFrame(unsigned char *imagedata, int imagesize, int linesize);
        int getFrame(std::queue<AVData> &dataQueue);
        ~FFMPEGCodec();

    private:
        FFMPEGOption _FFMPEOption;
        struct AVCodecDep
        {
            AVPacket Packet;
            AVFrame *frameInput;
            AVCodec *EncoderBase;
            AVCodecContext *Encoder;
            SwsContext *DataConvertor;
        } AVCD;
    };
}

FFMPEGTools::FFMPEGCodec::FFMPEGCodec(FFMPEGOption optionI)
{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif

    _FFMPEOption = optionI;
    AVCD.EncoderBase = avcodec_find_encoder(_FFMPEOption.OutputFormat);
    AVCD.Encoder = avcodec_alloc_context3(AVCD.EncoderBase);
    AVCD.Encoder->codec_tag = 0;
    AVCD.Encoder->codec_id = _FFMPEOption.OutputFormat;
    AVCD.Encoder->codec_type = AVMEDIA_TYPE_VIDEO;
    AVCD.Encoder->width = _FFMPEOption.IOWidth;
    AVCD.Encoder->height = _FFMPEOption.IOHeight;
    AVCD.Encoder->gop_size = _FFMPEOption.OBuffer;
    AVCD.Encoder->pix_fmt = AV_PIX_FMT_YUV420P;
    AVCD.Encoder->framerate = {_FFMPEOption.OFrameRate, 1};
    AVCD.Encoder->time_base = av_inv_q({_FFMPEOption.OFrameRate, 1});
    AVCD.Encoder->bit_rate = _FFMPEOption.OBitRate;
    AVCD.Encoder->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    AVDictionary *TmpCodecOptions = nullptr;
    av_dict_set(&TmpCodecOptions, "profile", _FFMPEOption.CodecProfile, 0);
    av_dict_set(&TmpCodecOptions, "preset", "ultrafast", 0);
    av_dict_set(&TmpCodecOptions, "tune", "zerolatency", 0);
    avcodec_open2(AVCD.Encoder, AVCD.EncoderBase, &TmpCodecOptions);

    AVCD.DataConvertor = sws_getContext(_FFMPEOption.IOWidth, _FFMPEOption.IOHeight,
                                        _FFMPEOption.TargetFormat,
                                        _FFMPEOption.IOWidth, _FFMPEOption.IOHeight,
                                        AV_PIX_FMT_YUV420P,
                                        SWS_FAST_BILINEAR, NULL, NULL, NULL);

    av_init_packet(&AVCD.Packet);
    AVCD.frameInput = av_frame_alloc();
    av_image_alloc(AVCD.frameInput->data, AVCD.frameInput->linesize, _FFMPEOption.IOWidth,
                   _FFMPEOption.IOHeight, AV_PIX_FMT_YUV420P, 1);
    AVCD.frameInput->width = _FFMPEOption.IOWidth;
    AVCD.frameInput->height = _FFMPEOption.IOHeight;
    AVCD.frameInput->format = static_cast<int>(AVCD.Encoder->pix_fmt);
}

void FFMPEGTools::FFMPEGCodec::pushFrame(unsigned char *imagedata, int imagesize, int linesize)
{
    sws_scale(AVCD.DataConvertor, &imagedata, &linesize, 0,
              _FFMPEOption.IOHeight, AVCD.frameInput->data, AVCD.frameInput->linesize);

    avcodec_send_frame(AVCD.Encoder, AVCD.frameInput);
}

int FFMPEGTools::FFMPEGCodec::getFrame(std::queue<AVData> &dataQueue)
{
    int codec_signal = 0;
    while (codec_signal != AVERROR(EAGAIN) && codec_signal != AVERROR_EOF)
    {
        codec_signal = avcodec_receive_packet(AVCD.Encoder, &AVCD.Packet);
        //
        if (codec_signal != AVERROR(EAGAIN) && codec_signal != AVERROR_EOF)
        {
            if ((AVCD.Packet.data[4] & 0x1f) == 5)
            {
                AVData dataSPS(_FFMPEOption.IOWidth, _FFMPEOption.IOHeight, AVCD.Encoder->extradata_size);
                //
                std::copy(AVCD.Encoder->extradata, AVCD.Encoder->extradata + AVCD.Encoder->extradata_size, dataSPS.data);
                //
                dataQueue.push(dataSPS);
            }

            AVData data(_FFMPEOption.IOWidth, _FFMPEOption.IOHeight, AVCD.Packet.size);
            //
            std::copy(AVCD.Packet.data, AVCD.Packet.data + AVCD.Packet.size, data.data);
            //
            dataQueue.push(data);
        }
    }
    return codec_signal;
};

FFMPEGTools::FFMPEGCodec::~FFMPEGCodec()
{
    avcodec_close(AVCD.Encoder);
    sws_freeContext(AVCD.DataConvertor);
    av_packet_unref(&AVCD.Packet);
    av_frame_free(&AVCD.frameInput);
}
