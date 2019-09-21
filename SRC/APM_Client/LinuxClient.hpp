#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#endif
#include <opencv2/opencv.hpp>
#include "../ToolsInclude/M_Tools.hpp"

namespace APMClient
{
class GUIClient
{
public:
#ifdef linux
    void RecvShow(std::string ipaddress, int localport, int remoteport);
#endif
};
} // namespace APMClient
