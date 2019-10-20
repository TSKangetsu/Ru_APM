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
	class MatDeal
	{
	public:
		cv::Mat FrameMotionDEBlur(cv::Mat inputFrame);
		cv::Mat FrameOFFDEBlur(cv::Mat inputFrame);
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

		struct DeBlur_Config
		{
			int OFF_Radiu = 1;
			int OFF_SNR = 1000;
			int MODE_LEN = 0;
			int MODE_THETA = 0;
			int MODE_SNR = 200;
		}DeBlur_Config;
	};

	class DnnModule
	{
	public:
		DnnModule();
		//return a marked image
		cv::Mat MatDnnDeal(cv::Mat inputFrame);
		//output image detected object datas
		int MatDnnDeal(cv::Mat inputFrame, float*& Data);
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

			bool Enable_FPS_Detect = true;
		} CV_Config;
	};

	class MarkOutModule
	{
	public:
		MarkOutModule();
		cv::Mat ColorCut(cv::Mat InputArray);
		int ImgMarkOut(cv::Mat InputArrayRanged);
		struct Markout_Args
		{
			cv::Scalar Color_Range[2];
		}Markout_Args;
	};

	class FramePost
	{
	public:
#ifdef linux
		int FramePostNet(int startCode);
#endif
		FramePost();

		FramePost(int frameBufferCount);
		bool Asyncprograssing = true;
		FrameBuffer<cv::Mat> AsyncCamBuffer;

		template<class _Tp>
		int CameraOutput(_Tp startFlag)
		{
			cv::Mat CatchTMP;
			cv::VideoCapture VideoCatch(startFlag);
			VideoCatch.set(cv::CAP_PROP_FRAME_HEIGHT, CameraConfig.Camera_Height);
			VideoCatch.set(cv::CAP_PROP_FRAME_WIDTH, CameraConfig.Camera_Width);
			VideoCatch.set(cv::CAP_PROP_BUFFERSIZE, CameraConfig.Camera_Buff);
			cv::namedWindow("NormalCamera", cv::WINDOW_NORMAL);
			cv::setWindowProperty("NormalCamera", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
			if (!VideoCatch.isOpened())
			{
				std::cout << "\033[35m[CameraStatus] camera start failed\033[0m\n";
				return -1;
			}
			else
			{
				while (true)
				{
					VideoCatch >> CatchTMP;
					cv::imshow("NormalCamera", CatchTMP);
					if (cv::waitKey(1) == 'q')
						break;
				}
			}
		}

		template<class _Tp>
		int CameraDNNOutput(_Tp startFlag)
		{
			cv::Mat CatchTMP;
			cv::VideoCapture VideoCatch(startFlag);
			VideoCatch.set(cv::CAP_PROP_FRAME_HEIGHT, CameraConfig.Camera_Height);
			VideoCatch.set(cv::CAP_PROP_FRAME_WIDTH, CameraConfig.Camera_Width);
			VideoCatch.set(cv::CAP_PROP_BUFFERSIZE, CameraConfig.Camera_Buff);
			cv::namedWindow("DnnOutput", cv::WINDOW_NORMAL);
			cv::setWindowProperty("DnnOutput", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
			CameraCOM::DnnModule ModuleSet;
			if (!VideoCatch.isOpened())
			{
				std::cout << "\033[35m[CameraStatus] camera start failed\033[0m\n";
				return -1;
			}
			else
			{
				while (true)
				{
					VideoCatch >> CatchTMP;
					CatchTMP = ModuleSet.MatDnnDeal(CatchTMP);
					cv::imshow("DnnOutput", CatchTMP);
					if (cv::waitKey(1) == 'q')
						break;
				}
			}
		}

	private:
		struct CameraConfig
		{
			int Camera_Buff;
			int Camera_Width;
			int Camera_Height;
		}CameraConfig;

	};


} // namespace CameraCOM