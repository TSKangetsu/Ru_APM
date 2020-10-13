#include <csignal>
#include <iostream>
#include <unistd.h>
#include "ToolsInclude/M_Tools.hpp"
#include "APMController/M_APMMain.hpp"
#include "CameraInclude/M_Camera.hpp"

int main()
{
	CameraCOM::FramePost mainPost;
	mainPost.CameraDNNOutput(0);
}