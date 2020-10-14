#include "VisionBaseExec.hpp"

int VideoServer::VideoServerReInit(VideoConfig &config)
{
    if (PusherThread.joinable())
    {
    }
    else
    {
        PusherThread = std::thread([&] {
            CameraInfo.ImgHeight = config.Height;
            CameraInfo.ImgWidth = config.Width;
            CameraInfo.Is_fastMode = true;
            CameraInfo.FrameBuffer = 1;
            CameraDevice = new V4L2Tools::V4L2Drive(config.CameraDevice, CameraInfo);
            if (config.VideoPushMode == PushMode::RTMP)
            {
                RTMPInfo._flag_Height = config.Height;
                RTMPInfo._flag_Width = config.Width;
                RTMPInfo._flag_BufferSize = 2;
                RTMPServer = new RTMPPusher(RTMPInfo);
                RTMPServer->RTMPEncodecerInit();
            }
            else if (config.VideoPushMode == PushMode::MJPEG)
            {
                mJPEGServer = new MJPEGServer();
                mJPEGServer->MJPEGWaitIncome();
            }
            else if (config.VideoPushMode == X11Display)
            {
                cv::namedWindow("LiveVideo", cv::WINDOW_NORMAL);
                cv::setWindowProperty("LiveVideo", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
            }
            //
            ProcessFlag = true;
            while (ProcessFlag)
            {
                IMGTmp = CameraDevice->V4L2Read();
                IMG = cv::Mat(CameraInfo.ImgHeight, CameraInfo.ImgWidth, CV_8UC3, IMGTmp);
                if (config.VideoPushMode == PushMode::RTMP)
                {
                    RTMPServer->CVMattToAvframe(&IMG, RTMPFrame);
                    RTMPServer->RTMPSender(RTMPFrame);
                }
                else if (config.VideoPushMode == PushMode::MJPEG)
                {
                    std::vector<uchar> buf;
                    cv::imencode(".jpg", IMG, buf, std::vector<int>());
                    std::string content(buf.begin(), buf.end());
                    mJPEGServer->MJPEGContentSender(content);
                }
                else if (config.VideoPushMode == X11Display)
                {
                    cv::imshow("LiveVideo", IMG);
                    cv::waitKey(10);
                }
            }
            //
            if (config.VideoPushMode == PushMode::RTMP)
            {
                RTMPServer->~RTMPPusher();
            }
            else if (config.VideoPushMode == PushMode::MJPEG)
            {
            }
            else if (config.VideoPushMode == X11Display)
            {
            }
        });
        return 0;
    }
}
