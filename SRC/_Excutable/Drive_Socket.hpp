#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <functional>
#include <thread>
#include <sys/stat.h>
#ifdef NETDEBUG
#include <iostream>
#endif
#define SYSMAXConnection sizeof(fd_set)

template <bool Block>
class uSocket
{
public:
    friend class SocketAsyncServer;
    uSocket()
    {
        m_sock = 0;
        memset(&m_addr, 0, sizeof(m_addr));
    };
    ~uSocket()
    {
        release();
    };
    bool Create()
    {
        m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (!Is_valid())
        {
            return false;
        }
        if (!Block)
            setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)1, sizeof((char *)1));
#ifdef NETDEBUG
        std::cout << "\033[32m[SocketInfo]Socket create sucess\033[0m\n";
#endif
        return true;
    };
    bool Bind(std::string IPAddress = std::string("0.0.0.0"), int port = 27015)
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
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]Socket Bind error!!!!!!!!!!!!!!!!!!\033[0m\n";
#endif
            close(m_sock);
            return false;
        }
#ifdef NETDEBUG
        std::cout << "\033[32m[SocketInfo]Socket is working at ";
        std::cout << IPAddress << ":";
        std::cout << port;
        std::cout << "\033[0m\n";
#endif
        return true;
    };
    bool Listen(int maxConnections)
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
#ifdef NETDEBUG
        std::cout << "\033[32m[SocketInfo]Socket starting listen\033[0m\n";
#endif
        return true;
    };
    bool Connect(std::string host, int port)
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
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]HostName failed abort\033[0m\n";
#endif
            return false;
        }

        status = ::connect(m_sock, (sockaddr *)&m_addr, sizeof(m_addr));
        socklen_t addr_size = sizeof(struct sockaddr_in);
        getpeername(m_sock, (struct sockaddr *)&RemoteSockInfo, (socklen_t *)&addr_size);
        if (status == 0)
        {
#ifdef NETDEBUG
            std::cout << "\033[32m[SocketInfo]Socket connected to a server\033[0m\n";
#endif
            return true;
        }
        else
        {
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]Remote Server could not connect\033[0m\n";
#endif
            return false;
        }
    };
    void GetRemoteInfo(std::string &RemoteIPAddr, int &RemotePort)
    {
        RemoteIPAddr = inet_ntoa(RemoteSockInfo.sin_addr);
        RemotePort = ntohs(RemoteSockInfo.sin_port);
    }

    template <typename SocketType>
    bool Accept(SocketType &newSocket)
    {
        int addr_length = sizeof(m_addr);
        newSocket.m_sock = accept(m_sock, (sockaddr *)&m_addr, (socklen_t *)&addr_length);
        getpeername(newSocket.m_sock, (struct sockaddr *)&newSocket.RemoteSockInfo, (socklen_t *)&addr_length);
        if (newSocket.m_sock <= 0)
        {
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]Remote Stop connect\033[0m\n";
#endif
            return false;
        }
        else
        {
#ifdef NETDEBUG
            std::cout << "\033[32m[SocketInfo]Socket recving a new connect\033[0m\n";
#endif

            return true;
        }
    };

    template <typename _Tp>
    bool Send(_Tp *data, int sendSize)
    {
        int status = send(m_sock, data, sendSize, 0);
        if (status == -1)
        {
#if NETDEBUG
            std::cout << "\033[31m[SocketInfo]Send data failed ! status code -1\033[0m\n";
#endif
            return false;
        }
        else
        {
#ifdef NETDEBUG
            std::cout << "\033[32m[SocketInfo]data has been send\033[0m\n";
#endif
            return true;
        }
    }

    template <typename _Tp>
    bool Recv(_Tp *&data, int recvSize)
    {
        _Tp buffer[recvSize + 1];
        memset(buffer, 0, recvSize + 1);
        int status = recv(m_sock, buffer, recvSize, 0);
        if (status == -1)
        {
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]Recving failed ! or recvSize error? status code is -1\033[0m\n";
#endif
            return false;
        }
        else if (status == 0)
        {
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]Recving failed ! or recvSize error?status code is 0\033[0m\n";
#endif
            return false;
        }
        else
        {
            data = buffer;
#ifdef NETDEBUG
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
    struct sockaddr_in RemoteSockInfo;
    bool Is_valid() const { return m_sock != -1; }
    int GetSockFD() { return m_sock; }
    void release()
    {
        if (Is_valid())
        {
            close(m_sock);
            m_sock = 0;
            socklen_t addr_size = sizeof(struct sockaddr_in);
            getpeername(m_sock, (struct sockaddr *)&RemoteSockInfo, (socklen_t *)&addr_size);
#ifdef NETDEBUG
            std::cout << "\033[31m[SocketInfo]Socket Close\033[0m\n";
#endif
        }
    }
};

typedef uSocket<true> Socket;
typedef uSocket<false> sSocket;

/*Usage:
    SocketAsyncServer().SocketServer("0.0.0.0", 27015, SYSMAXConnection)
            .OnConnection(4096, [](auto *req) {
                int Port;
                std::string IPAddr;
                std::cout << "\033[32m[SocketInfo]New connection incomming :";
                req->GetRemoteInfo(IPAddr, Port);
                std::cout << IPAddr << ":" << Port << "\033[0m\n";
            })
            .OnMessage([](auto *req, auto *data) {
                int Port;
                std::string IPAddr;
                req->GetRemoteInfo(IPAddr, Port);
                std::cout << "\033[32m[SocketInfo]DataIncoming From " << IPAddr << ":" << Port << " is \033[0m" << data << "\n";
                req->Send("=-=", 512);
            })
            .OnDisConnect([](auto *req) {
                int Port;
                std::string IPAddr;
                req->GetRemoteInfo(IPAddr, Port);
                std::cout << "\033[31m[SocketInfo]Client Disconnect from " << IPAddr << ":" << Port << "\033[0m\n";
            })
            .Run()
            .Wait();*/
class SocketAsyncServer
{
public:
    SocketAsyncServer(){};
    // Disallow copying, only move
    SocketAsyncServer(const SocketAsyncServer &other) = delete;

    SocketAsyncServer &&SocketServer(std::string iPAddress, int localPort, int maxConnection)
    {
        IsRun = false;
        IPAddr = iPAddress;
        LocalPort = localPort;
        MaxConnection = maxConnection;
        MainSocks = new sSocket();

        AcceptThread = std::thread([&]
                                   {
                                       do
                                       {
                                           usleep(200000);
                                       } while (!IsRun);

                                       sSocket SubSocks[MaxConnection];
                                       MainSocks->Create();
                                       MainSocks->Bind(IPAddr, LocalPort);
                                       MainSocks->Listen(MaxConnection);

                                       while (true)
                                       {
                                           FD_ZERO(&ServerFD);
                                           FD_SET(MainSocks->GetSockFD(), &ServerFD);
                                           Max_SD = MainSocks->GetSockFD();
                                           for (size_t i = 0; i < MaxConnection; i++)
                                           {
                                               if (SubSocks[i].GetSockFD() > 0)
                                                   FD_SET(SubSocks[i].GetSockFD(), &ServerFD);
                                               if (SubSocks[i].GetSockFD() > Max_SD)
                                                   Max_SD = SubSocks[i].GetSockFD();
                                           }
                                           int activity = select(Max_SD + 1, &ServerFD, NULL, NULL, NULL);
                                           for (size_t i = 0; i <= Max_SD && activity > 0; i++)
                                           {
                                               if (FD_ISSET(i, &ServerFD))
                                               {
                                                   activity--;
                                                   if (i == MainSocks->GetSockFD())
                                                   {
                                                       for (size_t i = 0; i < MaxConnection; i++)
                                                       {
                                                           if (!(SubSocks[i].GetSockFD() > 0))
                                                           {
                                                               bool AcceptSuccess = MainSocks->Accept(SubSocks[i]);
                                                               OnConnectionFunction(&SubSocks[i]);
                                                               break;
                                                           }
                                                       }
                                                   }
                                                   else
                                                   {
                                                       for (size_t s = 0; s < MaxConnection; s++)
                                                       {
                                                           if (SubSocks[s].GetSockFD() == i)
                                                           {
                                                               struct stat buf;
                                                               if (!SubSocks[s].Recv(RecvDataBuff, RecvMaxSize))
                                                               {
                                                                   OnDisConnectFunction(&SubSocks[s]);
                                                                   SubSocks[s].~sSocket();
                                                               }
                                                               else
                                                               {
                                                                   OnMessageFunction(&SubSocks[s], RecvDataBuff);
                                                               }
                                                           }
                                                       }
                                                   }
                                               }
                                           }
                                       }
                                   });
        return std::move(*this);
    };

    SocketAsyncServer &&OnConnection(int recvMaxSize, std::function<void(sSocket *)> &&CustomFunction)
    {
        RecvMaxSize = recvMaxSize;
        OnConnectionFunction = CustomFunction;
        return std::move(*this);
    }

    SocketAsyncServer &&OnMessage(std::function<void(sSocket *, char *data)> &&CustomFunction)
    {
        OnMessageFunction = CustomFunction;
        return std::move(*this);
    }

    SocketAsyncServer &&OnDisConnect(std::function<void(sSocket *)> &&CustomFunction)
    {
        OnDisConnectFunction = CustomFunction;
        return std::move(*this);
    }

    SocketAsyncServer &&Run()
    {
        IsRun = true;
        return std::move(*this);
    }
    //Notice: This will block you thread
    SocketAsyncServer &&Wait()
    {
        AcceptThread.join();
        return std::move(*this);
    }

private:
    int Max_SD;
    bool IsRun;
    int LocalPort;
    int MaxConnection;
    std::string IPAddr;

    int RecvMaxSize;
    char *RecvDataBuff;

    fd_set ServerFD;
    sSocket *MainSocks;

    std::thread AcceptThread;
    std::function<void(sSocket *)> OnConnectionFunction;
    std::function<void(sSocket *, char *data)> OnMessageFunction;
    std::function<void(sSocket *)> OnDisConnectFunction;
};