#include "M_Camera.hpp"
int CameraCOM::FramePost::FramePostNet(int startCode)
{
    cv::Mat CatchTMP;
    cv::VideoCapture VideoCatch(startCode);
    Base::Socket PostBind;
    Base::Socket SockSub;
    int imRows, imCols, imSize, imTrueSize, imType;
    unsigned char *imData;

    // VideoCatch.open(startCode);
    if (!VideoCatch.isOpened())
    {
        std::cout << "\033[35m[CameraStatus] camera start failed\033[0m\n";
        return -1;
    }
    else
    {
        VideoCatch >> CatchTMP;
        Base::TransTools::MatToByte(CatchTMP, imData, imRows, imCols, imSize, imTrueSize, imType);
        int imInfo[5] = {imRows, imCols, imSize, imTrueSize, imType};
        PostBind.SocketServer(SockSub, 10086, 2);
        if (SockSub.Send(imInfo, 100)) //step 1
        {
            std::cout << "\033[35m[CameraStatus] camera start to post\033[0m\n";
            while (true)
            {
                VideoCatch >> CatchTMP;
                Base::TransTools::MatToByte(CatchTMP, imData, imRows, imCols, imSize, imTrueSize, imType);
                SockSub.Send(imData, imTrueSize); //step 3
            }
        }
        else
        {
            std::cout << "\033[31m[CameraStatus]camera info send failed \033[0m\n";
            return -1;
        }
        return 0;
    }
}

int CameraCOM::FramePost::CameraCheck(int startCode)
{
    cv::Mat CatchTMP;
    cv::VideoCapture VideoCatch(startCode);
    VideoCatch.set(cv::CAP_PROP_FRAME_WIDTH, 300);
    VideoCatch.set(cv::CAP_PROP_FRAME_HEIGHT, 300);
    VideoCatch.set(cv::CAP_PROP_BUFFERSIZE, 1);

    CameraCOM::CnnCaculate ins("./Data/vino-banketFP32/frozen_inference_graph.xml", "./Data/vino-banketFP32/frozen_inference_graph.bin", 0, 0.4, cv::dnn::DNN_BACKEND_INFERENCE_ENGINE, cv::dnn::DNN_TARGET_CPU);
    if (!VideoCatch.isOpened())
    {
        std::cout << "\033[35m[CameraStatus] camera start failed\033[0m\n";
        return -1;
    }
    else
    {
        sleep(2);
        while (true)
        {
            VideoCatch >> CatchTMP;
            cv::Mat inss = ins.MatCnn(CatchTMP, 300, 300);
            imshow("test", inss);
            if (cv::waitKey(1) == 'q')
                break;
        }
    }
}
