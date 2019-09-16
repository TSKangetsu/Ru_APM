#pragma once
#ifndef MCAM_H
#define MCAM_H
#endif
#include "../ToolsInclude/M_Tools.hpp"
#include "../ConfigSetter/M_Config.hpp"
#include "../_thirdparty/json/single_include/nlohmann/json.hpp"
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

	DnnModule();
	cv::Mat MatDnnDeal(cv::Mat inputFrame);
private:
	void DnnModuleSet();
	cv::dnn::Net NetInside;

	struct CV_Config
	{
		int Camera_Buff;
		int Camera_Width;
		int Camera_Height;

		int Model_Type;

		int Blob_Scalar[3];
		int Blob_Size[2];

		float confidence_threshold;

		int Preferable_Backend;
		int Preferable_Target;

		std::string File_args1;
		std::string File_args2;

	}CV_Config;
};
} // namespace CameraCOM