#include "LinuxClient.hpp"

void APMClient::GUIClient::RecvShow(std::string ipaddress, int localport, int remoteport)
{
    Base::Socket sock;
    cv::Mat imMat;
    unsigned char *imData;
    int imRows, imCols, imSize, imTrueSize, imType;
    int *imInfo;
    sock.SocketClient(ipaddress, localport, remoteport);
    sock.Recv(imInfo, 100);
    //
    imRows = imInfo[0];
    imCols = imInfo[1];
    imSize = imInfo[2];
    imTrueSize = imInfo[3];
    imType = imInfo[4];
    //
    int *comfirmFlag = new int[2];
    comfirmFlag[1] = 1;
    sleep(5);
    sock.Send(comfirmFlag, 10);
    while (true)
    {
        sock.Recv(imData, imTrueSize);
        imMat = Base::TransTools::ByteToMat(imData, imRows, imCols, imType);
        cv::imshow("test", imMat);
    }
}