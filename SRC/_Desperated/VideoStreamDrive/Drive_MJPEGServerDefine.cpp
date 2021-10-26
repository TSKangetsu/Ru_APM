#include "Drive_MJPEGServer.hpp"

MJPEGServer::MJPEGServer()
{
    IsConnected = false;
    _VideoNetStream = new Socket();
    _ControllNetStream = new Socket();
    _VideoNetStream->Create();
    _VideoNetStream->Bind();
    _VideoNetStream->Listen(1);
}

int MJPEGServer::MJPEGWaitIncome()
{
    if (_VideoNetStream->Accept(_VideoNetStreamSub))
    {
        _VideoNetStreamSub.Recv(InputBuffer, RECVConrtentBufferSize);
        if (_VideoNetStreamSub.Send(MJPEGHeader.c_str(), MJPEGHeader.size()))
        {
            IsConnected = true;
        }
        else
        {
            IsConnected = false;
            _VideoNetStreamSub.~Socket();
        }
    }

    return 0;
}

bool MJPEGServer::MJPEGContentSender(std::string content)
{
    if (IsConnected)
    {
        if (_VideoNetStreamSub.Send(MJPEGContentHeader.c_str(), MJPEGContentHeader.size()))
        {
            if (_VideoNetStreamSub.Send(content.c_str(), content.size()))
            {
                if (_VideoNetStreamSub.Send(MJPEGContentTail.c_str(), MJPEGContentTail.size()))
                {
                    return true;
                }
            }
        }
    }

    return false;
}