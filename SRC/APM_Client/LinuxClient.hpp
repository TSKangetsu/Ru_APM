#pragma once
#include <opencv2/opencv.hpp>
#include "../ToolsInclude/M_Tools.hpp"

namespace APMClient
{
class GUIClient
{
public:
    void RecvShow(std::string ipaddress, int localport, int remoteport);
};
} // namespace APMClient
