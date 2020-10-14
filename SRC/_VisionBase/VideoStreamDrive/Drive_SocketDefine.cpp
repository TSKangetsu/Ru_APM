#include "Drive_Socket.hpp"

Socket::Socket()
{
    memset(&m_addr, 0, sizeof(m_addr));
#ifdef DEBUG
    std::cout << "\033[32m[SocketInfo]Socket got a initialization\033[0m\n";
#endif
}

Socket::~Socket()
{
    if (Is_valid())
    {
        close(m_sock);
#ifdef DEBUG
        std::cout << "\033[31m[SocketInfo]Socket Close\n";
#endif
    }
}

bool Socket::Create()
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
}

bool Socket::Bind(std::string IPAddress, int port)
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
}

bool Socket::Listen(int maxConnections)
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
}

bool Socket::Accept(Socket &newSocket)
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
}

bool Socket::Connect(std::string host, const int port)
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
}

void Socket::SocketServer(Socket &socketSub, std::string IPAddress, int localPort, int maxConnection)
{
    Create();
    Bind(IPAddress, localPort);
    Listen(maxConnection);
    Accept(socketSub);
}

void Socket::SocketClient(std::string LocalIPAddress, std::string RemoteIPAddress, int localPort, int remotePort)
{
    Create();
    Bind(LocalIPAddress, localPort);
    Connect(RemoteIPAddress, remotePort);
}