#pragma once
#include "../../_Excutable/Drive_Socket.hpp"

class MJPEGServer
{
public:
    MJPEGServer();
    int MJPEGWaitIncome();
    bool MJPEGContentSender(std::string content);

private:
    char *InputBuffer;
    bool IsConnected;
    bool IsControllerConnected;
    Socket _VideoNetStreamSub;
    Socket _ControllNetStreamSub;
    Socket *_VideoNetStream = NULL;
    Socket *_ControllNetStream = NULL;

    const int RECVConrtentBufferSize = 512;
    const std::string MJPEGHeader = "HTTP/1.0 200 OK\r\n"
                                    "Server: MJPEGSinglePusher\r\n"
                                    "Connection: close\r\n"
                                    "Max-Age: 0\r\n"
                                    "Expires: 0\r\n"
                                    "Cache-Control: no-cache, private\r\n"
                                    "Pragma: no-cache\r\n"
                                    "Content-Type: multipart/x-mixed-replace; "
                                    "boundary=--BoundaryString\r\n\r\n";
    const std::string MJPEGContentHeader = "--BoundaryString\r\nContent-type: image/jpg\r\nContent-Length: \r\n\r\n";
    const std::string MJPEGContentTail = "\r\n\r\n";
};