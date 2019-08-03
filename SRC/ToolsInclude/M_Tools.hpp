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
    void SocketServer(Socket &socketSub, int localPort, int maxConnection);
    void SocketClient(std::string IPAddress, int localPort, int remotePort);

    template <class _Tp>
    bool Send(_Tp *data, int sendSize)
    {
        int status = send(m_sock, data, sendSize, 0);
        if (status == -1)
        {
            std::cout << "\033[31m[SocketInfo]Send data failed ! status code -1\033[0m\n";
            return false;
        }
        else
        {
            std::cout << "\033[32m[SocketInfo]data has been send\033[0m\n";
            return true;
        }
    }

    template <class _Tp>
    bool Recv(_Tp *&data, int recvSize)
    {
        _Tp buffer[recvSize + 1];
        memset(buffer, 0, recvSize + 1);
        int status = recv(m_sock, buffer, recvSize, MSG_WAITALL);
        if (status == -1)
        {
            std::cout << "\033[31m[SocketInfo]Recving failed ! or recvSize error? status code is -1\033[0m\n";
            return false;
        }
        else if (status == 0)
        {
            std::cout << "\033[31m[SocketInfo]Recving failed ! or recvSize error?status code is 0\033[0m\n";
            return false;
        }
        else
        {
            data = buffer;
            std::cout << "\033[32m[SocketInfo]data recving! status code is ";
            std::cout << status;
            std::cout << "\033[0m\n";
            return true;
        }
    }

private:
    int m_sock;
    sockaddr_in m_addr;
    bool Is_valid() const { return m_sock != -1; }
}; // namespace Base
} // namespace Base