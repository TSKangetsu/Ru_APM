#ifdef linux
#include "ComClient.hpp"
void APMClient::GUIClient::RecvShow(std::string ipaddress, int localport, int remoteport)
{
    Base::Socket sock;
    cv::Mat imMat;
    unsigned char *imData;
    //-------- imInfo[0] is imRows [1]imCols [2]imSize [3]imTrueSize [4]imType--------//
    int *imInfo;
    sock.SocketClient(ipaddress, localport, remoteport);
    sock.Recv(imInfo, 100);
    while (true)
    {
        sock.Recv(imData, imInfo[3]);
        imMat = Base::TransTools::ByteToMat(imData, imInfo[0], imInfo[1], imInfo[4]);

        cv::imshow("test", imMat);
        if (cv::waitKey(10) == 'q')
            break;
    }
}
#endif