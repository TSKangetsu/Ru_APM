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
    inline Socket()
    {
        memset(&m_addr, 0, sizeof(m_addr));
#ifdef DEBUG
        std::cout << "\033[32m[SocketInfo]Socket got a initialization\033[0m\n";
#endif
    };
    inline ~Socket()
    {
        if (Is_valid())
        {
            close(m_sock);
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]Socket Close\n";
#endif
        }
    };
    inline bool Create()
    {
        m_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (!Is_valid())
        {
            return false;
        }
        int on = 1;
#ifdef DEBUG
        std::cout << "\033[32m[SocketInfo]Socket create sucess\033[0m\n";
#endif
        return true;
    };
    inline bool Bind(std::string IPAddress = std::string("0.0.0.0"), int port = 27015)
    {
        if (!Is_valid())
        {
            return false;
        }
        m_addr.sin_family = AF_INET;
        inet_pton(AF_INET, IPAddress.c_str(), &(m_addr.sin_addr));
        m_addr.sin_port = htons(port);
        int bind_return = bind(m_sock, (struct sockaddr *)&m_addr, sizeof(m_addr));
        if (bind_return == -1)
        {
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]Socket Bind error!!!!!!!!!!!!!!!!!!\033[0m\n";
#endif
            close(m_sock);
            return false;
        }
#ifdef DEBUG
        std::cout << "\033[32m[SocketInfo]Socket is working at ";
        std::cout << IPAddress << ":";
        std::cout << port;
        std::cout << "\033[0m\n";
#endif
        return true;
    };
    inline bool Listen(int maxConnections)
    {
        if (!Is_valid())
        {
            return false;
        }
        int listen_return = listen(m_sock, maxConnections);
        if (listen_return == -1)
        {
            return false;
        }
#ifdef DEBUG
        std::cout << "\033[32m[SocketInfo]Socket starting listen\033[0m\n";
#endif
        return true;
    };
    inline bool Accept(Socket &newSocket)
    {
        int addr_length = sizeof(m_addr);
        newSocket.m_sock = accept(m_sock, (sockaddr *)&m_addr, (socklen_t *)&addr_length);
        if (newSocket.m_sock <= 0)
        {
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]Remote Stop connect\033[0m\n";
#endif
            return false;
        }
        else
        {
#ifdef DEBUG
            std::cout << "\033[32m[SocketInfo]Socket recving a new connect\033[0m\n";
#endif
            return true;
        }
    };
    inline bool Connect(std::string host, int port)
    {
        if (!Is_valid())
        {
            return false;
        }
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        int status = inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);
        if (errno == EAFNOSUPPORT)
        {
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]HostName failed abort\033[0m\n";
#endif
            return false;
        }

        status = ::connect(m_sock, (sockaddr *)&m_addr, sizeof(m_addr));
        if (status == 0)
        {
#ifdef DEBUG
            std::cout << "\033[32m[SocketInfo]Socket connected to a server\033[0m\n";
#endif
            return true;
        }
        else
        {
#ifdef DEBUG
            std::cout << "\033[31m[SocketInfo]Remote Server could not connect\033[0m\n";
#endif
            return false;
        }
    };
    inline void SocketServer(Socket &socketSub, std::string IPAddress, int localPort, int maxConnection)
    {
        Create();
        Bind(IPAddress, localPort);
        Listen(maxConnection);
        Accept(socketSub);
    };
    inline void SocketClient(std::string LocalIPAddress, std::string RemoteIPAddress, int localPort, int remotePort)
    {
        Create();
        Bind(LocalIPAddress, localPort);
        Connect(RemoteIPAddress, remotePort);
    };

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