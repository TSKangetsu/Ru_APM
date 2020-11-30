#include <csignal>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include "APMController/M_APMMain.hpp"
#include "_VisionBase/VisionBaseExec.hpp"

int main()
{
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    double rate = 10.0;
    cv::VideoWriter vw;
    vw.open("videoTest.avi", cv::VideoWriter::fourcc('H', '2', '6', '4'), rate, cv::Size(1280, 720));
    cv::Mat tmpMat;
    cv::Mat vmpMat;
    CVInferConfig mt;
    CVInferEngine tmp(mt);
    int LoseFrameCount;
    int *ScreenData;
    int DetectionIndex;
    bool IsRecording;

    std::cout << "ready to start...\n";
    while (true)
    {
        cap.read(tmpMat);
        cv::imshow("test", tmpMat);
        vw << tmpMat;
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    tmpMat.release();
    vmpMat.release();
    vw.release();
    cv::destroyAllWindows();
    return 0;
}
