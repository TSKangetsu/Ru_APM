#include "Drive_RTMPPush.hpp"

RTMPPusher::RTMPPusher(RTMPPusherOption option)
{
    _flag_Option = option;
    _flag_Codec_FPS = {_flag_Option._flag_FPS, 1};
#ifdef DEBUG
    av_log_set_level(AV_LOG_DEBUG);
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
    avformat_network_init();
    _flag_Error = avformat_alloc_output_context2(&BaseFormatContext, nullptr, _flag_Option._flag_StreamFormat.c_str(), _flag_Option._flag_RTMPStreamURL.c_str());
    if (!(BaseFormatContext->oformat->flags & AVFMT_NOFILE))
    {
        _flag_Error = avio_open2(&BaseFormatContext->pb, _flag_Option._flag_RTMPStreamURL.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);
    }
}

bool RTMPPusher::RTMPEncodecerInit()
{
    BaseCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    BaseFormatStream = avformat_new_stream(BaseFormatContext, BaseCodec);
    BaseCodecContext = avcodec_alloc_context3(BaseCodec);
    BaseCodecContext->codec_tag = 0;
    BaseCodecContext->codec_id = AV_CODEC_ID_H264;
    BaseCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    BaseCodecContext->width = _flag_Option._flag_Width;
    BaseCodecContext->height = _flag_Option._flag_Height;
    BaseCodecContext->gop_size = _flag_Option._flag_BufferSize;
    BaseCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    BaseCodecContext->framerate = _flag_Codec_FPS;
    BaseCodecContext->time_base = av_inv_q(_flag_Codec_FPS);
    BaseCodecContext->bit_rate = _flag_Option._flag_Bitrate;
    if (BaseFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
    {
        BaseCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    _flag_Error = avcodec_parameters_from_context(BaseFormatStream->codecpar, BaseCodecContext);
    AVDictionary *_TmpCodecOptions = nullptr;
    av_dict_set(&_TmpCodecOptions, "profile", _flag_Option._flag_CodeCProfile.c_str(), 0);
    av_dict_set(&_TmpCodecOptions, "preset", "superfast", 0);
    av_dict_set(&_TmpCodecOptions, "tune", "zerolatency", 0);
    _flag_Error = avcodec_open2(BaseCodecContext, BaseCodec, &_TmpCodecOptions);
    BaseFormatStream->codecpar->extradata = BaseCodecContext->extradata;
    BaseFormatStream->codecpar->extradata_size = BaseCodecContext->extradata_size;
#ifdef DEBUG
    av_dump_format(BaseFormatContext, 0, _flag_Option._flag_RTMPStreamURL.c_str(), 1);
#endif
    _flag_Error = avformat_write_header(BaseFormatContext, nullptr);
}

AVFrame *RTMPPusher::CVMattToAvframe(cv::Mat *image, AVFrame *frame)
{
    int width = image->cols;
    int height = image->rows;
    int cvLinesizes[1];
    cvLinesizes[0] = image->step1();
    if (frame == NULL)
    {
        frame = av_frame_alloc();
        av_image_alloc(frame->data, frame->linesize, width, height, AVPixelFormat::AV_PIX_FMT_YUV420P, 1);
        frame->width = width;
        frame->height = height;
        frame->format = static_cast<int>(BaseCodecContext->pix_fmt);
    }
    SwsContext *conversion = sws_getContext(width, height, AVPixelFormat::AV_PIX_FMT_BGR24, width, height, BaseCodecContext->pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(conversion, &image->data, cvLinesizes, 0, height, frame->data, frame->linesize);
    sws_freeContext(conversion);
    return frame;
}

bool RTMPPusher::RTMPSender(AVFrame *TargetFrame)
{
    av_init_packet(&_TmpPacket);
    _flag_Error = avcodec_send_frame(BaseCodecContext, TargetFrame);
    _flag_Error = avcodec_receive_packet(BaseCodecContext, &_TmpPacket);
    _flag_Error = av_interleaved_write_frame(BaseFormatContext, &_TmpPacket);
    av_packet_unref(&_TmpPacket);
}

RTMPPusher::~RTMPPusher()
{
    av_write_trailer(BaseFormatContext);
    avcodec_close(BaseCodecContext);
    avio_close(BaseFormatContext->pb);
}