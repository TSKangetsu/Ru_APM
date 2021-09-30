#pragma once
#include <linux/videodev2.h>
#include <linux/v4l2-controls.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>
#include <sys/mman.h>
#include <errno.h>
#ifdef DEBUG
#include <iostream>
#endif
#define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

namespace V4L2Tools
{
    struct V4l2Info
    {
        int ImgWidth = 640;
        int ImgHeight = 380;
        int FrameRate = 30;
        int FrameBuffer = 4;
        /* 
        FastMode:You must set FrameBuffer to 1 to enable , 
        fastmode is not thread block mode,fast speed loop will cost lot of cpu usage ,controll you self;
        if it set to false or framebuffer > 1 , thread will block untill v4l2buffer is updated; 
        */
        bool Is_fastMode = false;
        bool Is_AutoSize = false;
        unsigned int PixFormat = V4L2_PIX_FMT_BGR24;
    };

    enum V4L2Error
    {
        _v4l2_query_error,
        _v4l2_vidioc_s_error,
        _v4l2_vidioc_g_error,
        _v4l2_vipram_s_error,
        _v4l2_reqbuff_error,
        _v4l2_querybuff_error,
        _v4l2_qbuf_error,
        _v4l2_vidioc_streamon_error,
        _v4l2_camread_error,
        _v4l2_autoset_error
    };

    class V4L2Drive
    {
    public:
        V4L2Drive(std::string Device, V4l2Info Info);
        int V4L2FDGetter() { return _flag_CameraFD; };
        bool V4L2Control(unsigned int id, int value);
        V4l2Info V4L2InfoGet() { return v4l2d; };
        unsigned char *V4L2Read();
        unsigned char *RGB24DataInit() { return (unsigned char *)malloc(v4l2d.ImgWidth * v4l2d.ImgHeight * 3 * sizeof(char)); };
        void yuyv2rgb24(const unsigned char *src, unsigned char *dest, int width, int height);
        ~V4L2Drive();

    private:
        void V4L2Log(int signal, int error);

        void **v4l2Buffers;
        int _flag_Error;
        int _flag_CameraFD;
        std::string _flag_TargetDevice;
        V4l2Info v4l2d;
        struct V4l2Dep
        {
            v4l2_control CameraContorl;
            v4l2_capability CameraInfo;
            v4l2_format CameraFormat;
            v4l2_requestbuffers CameraReqBuffer;
            v4l2_buf_type CameraType;
            v4l2_buffer CameraQBuffer;
            v4l2_buffer CameraBuffer;
            v4l2_fmtdesc CameraFMTInfo;
        } v4l2;
    };
} // namespace V4L2Tools
