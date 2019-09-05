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
	CnnCaculate(std::string pbFile, std::string pbtxtFile, int modelType, int PreferableBackend = 0, int PreferableTarget = 0, float confidence = 0.6, std::string config = "");
	cv::Mat MatCnn(cv::Mat inputFrame, int widSize, int heiSize);

private:
	cv::dnn::Net NetInside;
	cv::Mat blobImage;
	std::vector<std::string> deOut_names;
	float confidence_threshold;
	cv::Mat tmpGrayMat;

	cv::Mat itmpMat;
	cv::Mat resultsMat;
	std::vector<cv::Mat> deOut;

	size_t objIndex;
	float tl_x;
	float tl_y;
	float br_x;
	float br_y;
};
} // namespace CameraCOM