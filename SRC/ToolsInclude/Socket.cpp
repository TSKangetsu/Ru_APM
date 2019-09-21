
#include "M_Tools.hpp"
#ifdef windows
Base::Socket::Socket()
{
    memset(&socketSet, 0, sizeof(socketSet));
    std::cout << "\033[32m[SocketInfo]Socket got a initialization\033[0m\n";
}

Base::Socket::~Socket()
{
    if (!Is_valid())
    {
        closesocket(socketSet);
        std::cout << "\033[31m[SocketInfo]Socket Close\n";
    }
}

bool Base::Socket::Create()
{
    socketSet = socket(AF_INET, SOCK_STREAM, 0);
    // if (!Is_valid())
    // {
    //     std::cout << "\033[31m[SocketInfo]Socket create falied\033[0m\n";
    //     return false;
    // }
    std::cout << "\033[32m[SocketInfo]Socket create sucess\033[0m\n";
    return true;
}

bool Base::Socket::Bind(const int port)
{
    // if (!Is_valid())
    // {
    //     std::cout << "\033[31m[SocketInfo]Socket could not bind, please check you Socket is true start";
    //     return false;
    // }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int bind_return = bind(socketSet, (SOCKADDR *)&addr, sizeof(addr));
    // if (bind_return = SOCKET_ERROR)
    // {
    //     std::cout << "\033[31m[SocketInfo]Socket Bind error!!!!!!!!!!!!!!!!!!\033[0m\n";
    //     closesocket(socketSet);
    //     return false;
    // }
    std::cout << "\033[32m[SocketInfo]Socket is working at ";
    std::cout << port;
    std::cout << "\033[0m\n";
    return true;
}

bool Base::Socket::Bind(std::string ipAddr, const int port)
{
    // if (!Is_valid())
    // {
    //     std::cout << "\033[31m[SocketInfo]Socket could not bind, please check you Socket is true start";
    //     return false;
    // }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    int bind_return = bind(socketSet, (SOCKADDR *)&addr, sizeof(addr));
    if (bind_return = SOCKET_ERROR)
    {
        std::cout << "\033[31m[SocketInfo]Socket Bind error!!!!!!!!!!!!!!!!!!\033[0m\n";
        closesocket(socketSet);
        return false;
    }
    std::cout << "\033[32m[SocketInfo]Socket is working at ";
    std::cout << port;
    std::cout << "\033[0m\n";
    return true;
}

bool Base::Socket::Listen(int maxConnections)
{
    if (!Is_valid())
    {
        std::cout << "\033[031m[SocketInfo]Listennig port error\033[0m\n";
        return false;
    }
    int listen_return = listen(socketSet, maxConnections);
    if (listen_return == -1)
    {
        return false;
    }
    std::cout << "\033[32m[SocketInfo]Socket starting listen\033[0m\n";
    return true;
}

bool Base::Socket::Accept(Socket &newSocket)
{
    int addr_length = sizeof(socketSet);
    newSocket.socketSet = accept(socketSet, (SOCKADDR *)&socketSet, NULL);
    if (newSocket.socketSet <= 0)
    {
        std::cout << "\033[31m[SocketInfo]Remote Stop connect\033[0m\n";
        return false;
    }
    else
    {
        std::cout << "\033[32m[SocketInfo]Socket recving a new connect\033[0m\n";
        return true;
    }
}

bool Base::Socket::Connect(std::string host, const int port)
{
    if (!Is_valid())
    {
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int status = ::connect(socketSet, (SOCKADDR *)&addr, sizeof(addr));
    if (status == 0)
    {
        std::cout << "\033[32m[SocketInfo]Socket connected to a server\033[0m\n";
        return true;
    }
    else
    {
        std::cout << "\033[31m[SocketInfo]Remote Server could not connect\033[0m\n";
        return false;
    }
}
void Base::Socket::SocketServer(Socket &socketSub, int localPort, int maxConnection)
{
    Create();
    Bind(localPort);
    Listen(maxConnection);
    Accept(socketSub);
}

void Base::Socket::SocketClient(std::string IPAddress, int localPort, int remotePort)
{
    Create();
    Bind(localPort);
    Connect(IPAddress, remotePort);
}
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------//

#ifdef linux
Base::Socket::Socket()
{
    memset(&addr, 0, sizeof(addr));
    std::cout << "\033[32m[SocketInfo]Socket got a initialization\033[0m\n";
}

Base::Socket::~Socket()
{
    if (Is_valid())
    {
        close(m_sock);
        std::cout << "\033[31m[SocketInfo]Socket Close\n";
    }
}

bool Base::Socket::Create()
{
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (!Is_valid())
    {
        std::cout << "\033[31m[SocketInfo]Socket create falied\033[0m\n";
        return false;
    }
    std::cout << "\033[32m[SocketInfo]Socket create sucess\033[0m\n";
    return true;
}

bool Base::Socket::Bind(const int port)
{
    if (!Is_valid())
    {
        std::cout << "\033[31m[SocketInfo]Socket could not bind, please check you Socket is true start";
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    int bind_return = bind(m_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (bind_return == -1)
    {
        std::cout << "\033[31m[SocketInfo]Socket Bind error!!!!!!!!!!!!!!!!!!\033[0m\n";
        close(m_sock);
        return false;
    }
    std::cout << "\033[32m[SocketInfo]Socket is working at ";
    std::cout << port;
    std::cout << "\033[0m\n";
    return true;
}

bool Base::Socket::Listen(int maxConnections)
{
    if (!Is_valid())
    {
        std::cout << "\033[031m[SocketInfo]Listennig port error\033[0m\n";
        return false;
    }
    int listen_return = listen(m_sock, maxConnections);
    if (listen_return == -1)
    {
        return false;
    }
    std::cout << "\033[32m[SocketInfo]Socket starting listen\033[0m\n";
    return true;
}

bool Base::Socket::Accept(Socket &newSocket)
{
    int addr_length = sizeof(addr);
    newSocket.m_sock = accept(m_sock, (sockaddr *)&addr, (socklen_t *)&addr_length);
    if (newSocket.m_sock <= 0)
    {
        std::cout << "\033[31m[SocketInfo]Remote Stop connect\033[0m\n";
        return false;
    }
    else
    {
        std::cout << "\033[32m[SocketInfo]Socket recving a new connect\033[0m\n";
        return true;
    }
}

bool Base::Socket::Connect(std::string host, const int port)
{
    if (!Is_valid())
    {
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int status = inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    if (errno == EAFNOSUPPORT)
    {
        std::cout << "\033[31m[SocketInfo]HostName failed abort\033[0m\n";
        return false;
    }

    status = ::connect(m_sock, (sockaddr *)&addr, sizeof(addr));
    if (status == 0)
    {
        std::cout << "\033[32m[SocketInfo]Socket connected to a server\033[0m\n";
        return true;
    }
    else
    {
        std::cout << "\033[31m[SocketInfo]Remote Server could not connect\033[0m\n";
        return false;
    }
}

void Base::Socket::SocketServer(Socket &socketSub, int localPort, int maxConnection)
{
    Create();
    Bind(localPort);
    Listen(maxConnection);
    Accept(socketSub);
}

void Base::Socket::SocketClient(std::string IPAddress, int localPort, int remotePort)
{
    Create();
    Bind(localPort);
    Connect(IPAddress, remotePort);
}
#endif