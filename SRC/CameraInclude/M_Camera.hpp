#pragma once
#include "../ToolsInclude/M_Tools.hpp"
#include <string>
#include <iostream>
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
	//For X86 Device
	CnnCaculate(std::string args1,
				std::string args2,
				int modelType,
				int Backend = cv::dnn::DNN_BACKEND_DEFAULT,
				int Target = cv::dnn::DNN_TARGET_CPU);

	cv::Mat MatCnn(cv::Mat inputFrame,
				   int widSize,
				   int heiSize,
				   float confidence_threshold);

	//For Raspberry Intel NCS
	CnnCaculate();
	void MatCnnRaspi(int widSize, int heiSize);

private:
	cv::dnn::Net NetInside;
	float confidence_threshold;
};

class DnnModule
{
	struct Cv_Config
	{
		int Camera_Buff;
		int Camera_Width;
		int Camera_Height;
	};
}
} // namespace CameraCOM