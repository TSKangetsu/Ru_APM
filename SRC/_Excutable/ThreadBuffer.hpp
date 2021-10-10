#pragma once
#include <mutex>
#include <queue>
#include <opencv2/opencv.hpp>

template <typename T>
class FrameBuffer : public std::queue<T>
{
public:
    unsigned int frameCount;
    FrameBuffer() : frameCount(0) { mutex = new std::mutex(); };

    void pushFrame(const T &FrameBuffering)
    {
        std::lock_guard<std::mutex> lock(*mutex);

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
        std::lock_guard<std::mutex> lock(*mutex);
        T lastestFrame = this->front();
        this->pop();
        frameCount -= 1;
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
        std::lock_guard<std::mutex> lock(*mutex);
        while (!this->empty())
            this->pop();
    }

private:
    cv::TickMeter timeDec;
    std::mutex *mutex;
};