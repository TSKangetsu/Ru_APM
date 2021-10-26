#pragma once
#include <thread>
#include <opencv2/opencv.hpp>
#ifdef DEBUG
#include <iostream>
#endif

struct CVInferConfig
{
    int Model_Type = 1;
    int Blob_Scalar[3] = {100, 100, 100};
    int Blob_Size[2] = {300, 300};
    float Confidence_Threshold = 0.4;
    int Preferable_Backend = 0;
    int Preferable_Target = 3;
    std::string File_args1 = "../Data/vino-banketFP16/frozen_inference_graph.xml";
    std::string File_args2 = "../Data/vino-banketFP16/frozen_inference_graph.bin";
    bool Enable_FPS_Detect = true;
};

class CVInferEngine
{
public:
    CVInferEngine(CVInferConfig &config);

    cv::Mat CVINferMatAsyncBlock(int CAM);

    cv::Mat CVInferMatSync(cv::Mat inputFrame);

    int CVInferDataSync(cv::Mat inputFrame, float *Data, int dataSize);

private:
    cv::dnn::Net InferEngine;
    CVInferConfig InferConfig;

    template <typename T>
    class FrameBuffer : public std::queue<T>
    {
    public:
        unsigned int frameCount;
        FrameBuffer() : frameCount(0){};

        void pushFrame(const T &FrameBuffering)
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
};
