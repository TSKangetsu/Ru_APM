#ifndef Camera
#define Camera
#endif
#pragma once
#include "../ToolsInclude/M_Tools.hpp"
#include <opencv2/opencv.hpp>

namespace Camera
{
class FramePost
{
public:
    FramePost(){};
    ~FramePost(){};
    unsigned char *GetCameraInfo();
    int FramePostNet(int startCode);
};
} // namespace Camera