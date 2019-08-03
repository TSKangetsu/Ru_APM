#include "M_Tools.hpp"

Base::Socket::Socket()
{
    memset(&m_addr, 0, sizeof(m_addr));
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
        return false;
    }
    int on = 1;
    std::cout << "\033[32m[SocketInfo]Socket create sucess\033[0m\n";
    return true;
}

bool Base::Socket::Bind(const int port)
{
    if (!Is_valid())
    {
        return false;
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(port);
    int bind_return = bind(m_sock, (struct sockaddr *)&m_addr, sizeof(m_addr));
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
    int addr_length = sizeof(m_addr);
    newSocket.m_sock = accept(m_sock, (sockaddr *)&m_addr, (socklen_t *)&addr_length);
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
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    int status = inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);
    if (errno == EAFNOSUPPORT)
    {
        std::cout << "\033[31m[SocketInfo]HostName failed abort\033[0m\n";
        return false;
    }

    status = ::connect(m_sock, (sockaddr *)&m_addr, sizeof(m_addr));
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