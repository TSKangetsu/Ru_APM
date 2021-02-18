#include "VisionBaseExec.hpp"

int VideoServer::VideoServerReInit(VideoConfig &config)
{
    if (PusherThread.joinable())
    {
    }
    else
    {
        ProcessFlag = true;
        PusherThread = std::thread([&] {
            CameraInfo.ImgHeight = config.Height;
            CameraInfo.ImgWidth = config.Width;
            CameraInfo.FrameBuffer = 2;
            CameraDevice = new V4L2Tools::V4L2Drive(config.CameraDevice, CameraInfo);
            int main = CameraDevice->V4L2FDGetter();
            //
            CameraInfoSub.ImgHeight = config.SubHeight;
            CameraInfoSub.ImgWidth = config.SubWidth;
            CameraInfoSub.FrameBuffer = 2;
            CameraInfoSub.PixFormat = V4L2_PIX_FMT_YUYV;
            CameraDeviceSub = new V4L2Tools::V4L2Drive(config.CameraDeviceSub, CameraInfoSub);
            int Sub = CameraDeviceSub->V4L2FDGetter();
            SubIMG = cv::Mat(config.SubHeight - 4, config.SubWidth, CV_8UC3);
            CameraDeviceSub->V4L2Control(V4L2_CID_ZOOM_ABSOLUTE, 0x8005);
            usleep(200000);
            CameraDeviceSub->V4L2Control(V4L2_CID_ZOOM_ABSOLUTE, 0x8802);
            usleep(200000);
            CameraDeviceSub->V4L2Control(V4L2_CID_ZOOM_ABSOLUTE, 0x8000);
            usleep(200000);
            SubIMGTmp = CameraDeviceSub->V4L2Read();
            xtopt.IMGHeight = config.SubHeight;
            xtopt.IMGWidth = config.SubWidth;
            xtopt.CamMode = 5;
            XthermSDKReInit(xtopt, SubIMGTmp);
            SubIMGTmpRGB24 = CameraDeviceSub->RGB24DataInit();
            TempData = XthermSDKTempDataInit(config.SubWidth, config.SubHeight);
            IsCamInited = true;
            //
            InferConfig.File_args1 = "./Data/vino-banketFP16/frozen_inference_graph.xml";
            InferConfig.File_args2 = "./Data/vino-banketFP16/frozen_inference_graph.bin";
            InferEngine = new CVInferEngine(InferConfig);

            if (config.VideoPushMode == PushMode::RTMP)
            {
                RTMPInfo._flag_Height = config.Height;
                RTMPInfo._flag_Width = config.Width * 2;
                RTMPInfo._flag_BufferSize = 2;
                RTMPInfo._flag_Bitrate = 3000000;
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
            while (ProcessFlag)
            {
                IMGTmp = CameraDevice->V4L2Read();
                IMG = cv::Mat(CameraInfo.ImgHeight, CameraInfo.ImgWidth, CV_8UC3, IMGTmp);
                //
                if (config.VideoPushMode == PushMode::RTMP)
                {
                    if (!SubIMGFinal.empty())
                    {
                        cv::Mat IMGFinal;
                        cv::Mat SubIMGRE;
                        cv::resize(SubIMGFinal, SubIMGRE, cv::Size(640, 380));
                        cv::resize(IMG, IMG, cv::Size(640, 380));
                        cv::hconcat(SubIMGRE, IMG, IMGFinal);
                        IMGFinal = InferEngine->CVInferMatSync(IMGFinal);
                        RTMPFrame = RTMPServer->CVMattToAvframe(&IMGFinal, RTMPFrame);
                        RTMPServer->RTMPSender(RTMPFrame);
                    }
                }
                else if (config.VideoPushMode == PushMode::MJPEG)
                {
                    IMG = InferEngine->CVInferMatSync(IMG);
                    std::vector<uchar> buf;
                    cv::imencode(".jpg", IMG, buf, std::vector<int>());
                    std::string content(buf.begin(), buf.end());
                    mJPEGServer->MJPEGContentSender(content);
                }
                else if (config.VideoPushMode == X11Display)
                {
                    IMG = InferEngine->CVInferMatSync(IMG);
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

        VideoSub = std::thread([&] {
            while (ProcessFlag)
            {
                if (IsCamInited)
                {
                    SubIMGTmp = CameraDeviceSub->V4L2Read();
                    XthermSDKDataGet(xtopt, SubIMGTmp, SubIMGTmp, TempData);
                    CameraDeviceSub->yuyv2rgb24(SubIMGTmp, SubIMGTmpRGB24, config.SubWidth, config.SubHeight);
                    SubIMG = cv::Mat(config.SubHeight - 4, config.SubWidth, CV_8UC3, SubIMGTmpRGB24);
                    cv::cvtColor(SubIMG, SubIMG, cv::COLOR_RGB2BGR);
                    cv::putText(SubIMG, "+", cv::Point(config.SubWidth / 2, config.SubHeight / 2),
                                2, 0.4, cv::Scalar(0, 255, 0));
                    cv::putText(SubIMG, RoundNumber(TempData[0]), cv::Point(config.SubWidth / 2 - 20, config.SubHeight / 2 + 20),
                                2, 0.6, cv::Scalar(0, 255, 0));
                    cv::putText(SubIMG, "+", cv::Point(TempData[1], TempData[2]),
                                2, 0.4, cv::Scalar(255, 0, 0));
                    cv::putText(SubIMG, RoundNumber(TempData[3]), cv::Point(TempData[1] - 20, TempData[2] + 20),
                                2, 0.6, cv::Scalar(255, 0, 0));
                    cv::putText(SubIMG, "+", cv::Point(TempData[4], TempData[5]),
                                2, 0.4, cv::Scalar(0, 0, 255));
                    cv::putText(SubIMG, RoundNumber(TempData[6]), cv::Point(TempData[4] - 20, TempData[5] + 20),
                                2, 0.6, cv::Scalar(0, 0, 255));
                    SubIMGFinal = SubIMG;
                }
            }
        });
        return 0;
    }
}

void VideoRecorderTest()
{
         int DataCount;
        int MYDataSize;
        int Config = 0;
        int CountDown;
        int DetectionIndex;
        bool IsRecoring = true;
        float MYData[256 * 6];
        CVInferConfig myConfig;
        CVInferEngine *InferEngineSub = new CVInferEngine(myConfig);
        cv::Mat tmpMat;
        cv::VideoCapture cap(0);
        double rate = 10.0;
        cv::VideoWriter MyVideo;
        MyVideo.open("videoTest.avi", cv::VideoWriter::fourcc('H', '2', '6', '4'), rate, cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
        std::cout << "ready to start...\n";
        while (1)
        {
            cap.read(tmpMat);
            DataCount = InferEngineSub->CVInferDataSync(tmpMat, MYData, MYDataSize);
            cv::imshow("test", tmpMat);
            for (int r = 0; r < DataCount; r++)
            {
                DetectionIndex = MYData[r * 6];
                std::cout << DetectionIndex << "\n";
            }
            if (DetectionIndex == 1)
            {
                Config = Config + 1;
            }
            else
            {
                Config = 0;
            }
            IsRecoring = Config > 5;
            if (IsRecoring)
            {
                CountDown = 200;
            }
            if (CountDown > 1)
            {
                MyVideo << tmpMat;
                CountDown--;
            }
            if (cv::waitKey(1) == 'q')
            {
                break;
            }
        }
        tmpMat.release();
        MyVideo.release();
        cv::destroyAllWindows();
}