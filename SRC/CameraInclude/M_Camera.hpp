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

class DnnModule
{
public:
	//For X86 Device
	DnnModule(std::string args1,
				std::string args2,
				int modelType,
				int Backend = cv::dnn::DNN_BACKEND_DEFAULT,
				int Target = cv::dnn::DNN_TARGET_CPU);

	cv::Mat MatCnn(cv::Mat inputFrame,
				   int widSize,
				   int heiSize,
				   float confidence_threshold);

	void DnnModule_Test() {};
	void MatCnn_Test() {};

private:
	struct Cv_Config
	{
		int Camera_Buff;
		int Camera_Width;
		int Camera_Height;

		int Blob_Scalar[3];
		int Blob_Size[2];

		float confidence_threshold;

		int Preferable_Backend;
		int Preferable_Target;

		std::string File_args1;
		std::string File_args2;

	}CV_Config;

	cv::dnn::Net NetInside;
	float confidence_threshold;
};
} // namespace CameraCOM