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
#include <thread>

namespace CameraCOM
{
	class FramePost
	{
	public:
#ifdef linux
		FramePost() {};
		int FramePostNet(int startCode);
#endif
		int CameraCheck(int startCode);
		~FramePost() {};
	};

#ifdef Debuging
	class DeMothion
	{
		void calcPSF(cv::Mat& outframe, cv::Size filterSize, int R);
		void fftshift(const cv::Mat& inputFrame, cv::Mat& outputFrame);
		void filter2DFreq(const cv::Mat& inputImg, cv::Mat& outputImg, const cv::Mat& H);
		void calcWnrFilter(const cv::Mat& input_h_PSF, cv::Mat& output_G, double nsr);
	};
#endif

	class DnnModule
	{
	public:
		DnnModule();
		cv::Mat MatDnnDeal(cv::Mat inputFrame);
		void AsyncMatDnnDeal();

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

		} CV_Config;
	};

	template <typename T>
	class FrameBuffer : public std::queue<T>
	{
	public:
		unsigned int frameCount;
		FrameBuffer() : frameCount(0) {};

		void pushFrame(const T& FrameBuffering)
		{
			std::lock_guard<std::mutex> lock(mutex);

			std::queue<T>::push(FrameBuffering);
			frameCount += 1;
			if (frameCount == 1)
			{
				timeDec.reset();
				timeDec.start();
			}
		}

		T getFrame()
		{
			std::lock_guard<std::mutex> lock(mutex);
			T lastestFrame = this->front();
			this->pop();
			return lastestFrame;
		}

		float getDecFPS()
		{
			timeDec.stop();
			double fps = frameCount / timeDec.getTimeSec();
			timeDec.start();
			return static_cast<float>(fps);
		}

		void clearBuffer()
		{
			std::lock_guard<std::mutex> lock(mutex);
			while (!this->empty())
				this->pop();
		}

	private:
		cv::TickMeter timeDec;
		std::mutex mutex;
	};
} // namespace CameraCOM