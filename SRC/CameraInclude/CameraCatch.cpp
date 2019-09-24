#include "M_Camera.hpp"

#ifdef linux
int CameraCOM::FramePost::FramePostNet(int startCode)
{
    cv::Mat CatchTMP;
    cv::VideoCapture VideoCatch(startCode);
    Base::Socket PostBind;
    Base::Socket SockSub;
    int imRows, imCols, imSize, imTrueSize, imType;
    unsigned char *imData;

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
#endif


