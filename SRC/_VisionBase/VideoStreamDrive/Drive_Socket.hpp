#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>
#ifdef DEBUG
#include <iostream>
#endif
class Socket
{
public:
    Socket();
    ~Socket();
    bool Create();
    bool Bind(std::string IPAddres = std::string("0.0.0.0"), int port = 27015);
    bool Listen(int maxConnections);
    bool Accept(Socket &SocketSub);
    bool Connect(std::string host, int port);
    void SocketServer(Socket &socketSub, std::string IPAddress, int localPort, int maxConnection);
    void SocketClient(std::string LocalIPAddress, std::string RemoteIPAddress, int localPort, int remotePort);

    template <class _Tp>
    bool Send(_Tp *data, int sendSize)
    {
        int status = send(m_sock, data, sendSize, 0);
        if (status == -1)
        {
#if DEBUG
            std::cout << "\033[31m[SocketInfo]Send data failed ! status code -1\033[0m\n";
#endif
            return false;
        }
        else
        {
#ifdef DEBUG
            std::cout << "\033[32m[SocketInfo]data has been send\033[0m\n";
#endif
            return true;
        }
    }

    template <class _Tp>
    bool Recv(_Tp *&data, int recvSize)
    {
        _Tp buffer[recvSize + 1];
        memset(buffer, 0, recvSize + 1);
        int status = recv(m_sock, buffer, recvSize, 0);
        if (status == -1)
        {
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]Recving failed ! or recvSize error? status code is -1\033[0m\n";
#endif
            return false;
        }
        else if (status == 0)
        {
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]Recving failed ! or recvSize error?status code is 0\033[0m\n";
#endif
            return false;
        }
        else
        {
            data = buffer;
#ifdef DEBUG
            std::cout << "\033[32m[SocketInfo]data recving! status code is ";
            std::cout << status;
            std::cout << "\033[0m\n";
#endif
            return true;
        }
    }

private:
    int m_sock;
    sockaddr_in m_addr;
    bool Is_valid() const { return m_sock != -1; }
};