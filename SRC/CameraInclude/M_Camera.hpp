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
		int CameraCheck(int startCode);
		~FramePost() {};
#endif
	};

	class MatDeBlur
	{
	public:
		void FrameOFFDEBlur();
	private:
		//this is a OFF deblur function
		void CalcPSF(cv::Mat& outframe, cv::Size filterSize, int R);
		//this is a motion deblur function
		void CalcPSF(cv::Mat& outputframe, cv::Size filterSize, int len, double theta);
		void Fftshift(const cv::Mat& inputFrame, cv::Mat& outputFrame);
		void Filter2DFreq(const cv::Mat& inputFrame, cv::Mat& outputImg, const cv::Mat& H);
		void CalcWnrFilter(const cv::Mat& input_h_PSF, cv::Mat& output_G, double nsr);
		//here are motiondeblur function last
		void Edgetaper(const cv::Mat& inputFrame, cv::Mat& outputframe, double gamma = 5.0, double beta = 0.2);
	};

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