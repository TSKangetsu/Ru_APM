#ifndef TOOLS_H
#define TOOLS_H
#endif
#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <opencv2/opencv.hpp>

namespace Base
{
class TransTools
{
public:
    static unsigned char *MatToByte(cv::Mat imFrame, unsigned char *&imData, int &imRows, int &imCols, int &imSize, int &imTrueSize, int &imType);
    static cv::Mat ByteToMat(unsigned char *imByte, int imRows, int imCols, int imType);
    static unsigned char *StringToByte(std::string strData);
    static std::string ByteToString(unsigned char *byteData);
};

class Matcom
{
public:
    int ConvertSaveImg(std::string imName, cv::Mat imageData);
    cv::Mat ImageGet(std::string imdataName);
};

class Socket
{
public:
    Socket();
    ~Socket();
    bool Create();
    bool Bind(int port);
    bool Listen(int maxConnections);
    bool Accept(Socket &SocketSub);
    bool Connect(std::string host, int port);
    bool Send(unsigned char *data, int sendSize);
    bool Recv(unsigned char *&data, int recvSize);
    void SocketServer(Socket &socketSub, int localPort, int maxConnection);
    void SocketClient(std::string IPAddress, int localPort, int remotePort);

private:
    int m_sock;
    sockaddr_in m_addr;
    bool Is_valid() const { return m_sock != -1; }
};
} // namespace Base