#ifndef CameraM
#define CameraM
#endif
#pragma once
#include "../ToolsInclude/M_Tools.hpp"
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace CameraCOM
{
class FramePost
{
public:
	FramePost(){};
	int FramePostNet(int startCode);
	int CameraCheck(int startCode);
	~FramePost(){};
};

class CnnCaculate
{
public:
	CnnCaculate(std::string pbFile, std::string pbtxtFile, int modelType, float confidence = 0.5, int PreferableBackend = cv::dnn::DNN_BACKEND_DEFAULT, int PreferableTarget = cv::dnn::DNN_TARGET_CPU);
	cv::Mat MatCnn(cv::Mat inputFrame, int widSize, int heiSize);

private:
	cv::dnn::Net NetInside;
	float confidence_threshold;

	size_t objIndex;
	float tl_x;
	float tl_y;
	float br_x;
	float br_y;
};
} // namespace CameraCOM