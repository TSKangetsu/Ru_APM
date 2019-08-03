#include "M_Camera.hpp"
int Camera::FramePost::FramePostNet(int startCode)
{
    cv::Mat CatchTMP;
    cv::VideoCapture VideoCatch;
    Base::Socket PostBind;
    Base::Socket SockSub;
    int imRows, imCols, imSize, imTrueSize, imType;
    unsigned char *imData;

    VideoCatch.open(startCode);
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
        PostBind.SocketServer(SockSub, 1919, 2);
        if (SockSub.Send((unsigned char *)imInfo, 100))
        {
            unsigned char *data;
            SockSub.Recv(data, 100);
            if (data == (unsigned char *)400)
            {
                std::cout << "\033[35m[CameraStatus] camera info comfirm \033[0m\n";
                std::cout << "\033[35m[CameraStatus] camera start to post\033[0m\n";
                while (true)
                {
                    VideoCatch >> CatchTMP;
                    Base::TransTools::MatToByte(CatchTMP, imData, imRows, imCols, imSize, imTrueSize, imType);
                    SockSub.Send(imData, imTrueSize);
                }
            }
            else
            {
                std::cout << "\033[31m[CameraStatus] camera info do not comfirm , please retry agin later\033[0m\n";
                return -1;
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