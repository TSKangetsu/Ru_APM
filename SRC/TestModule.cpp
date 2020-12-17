#include <csignal>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include "APMController/M_APMMain.hpp"
#include "_VisionBase/VisionBaseExec.hpp"

int main()
{
    int DataCount;
    int MYDataSize;
    int Config = 0;
    int CountDown;
    int DetectionIndex;
    bool IsRecoring = true;
    float MYData[256 * 6];
    CVInferConfig myConfig;
    CVInferEngine *InferEngineSub = new CVInferEngine(myConfig);
    cv::Mat tmpMat;
    cv::VideoCapture cap(0);
    double rate = 10.0;
    cv::VideoWriter MyVideo;
    MyVideo.open("videoTest.avi", cv::VideoWriter::fourcc('H', '2', '6', '4'), rate, cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
    std::cout << "ready to start...\n";
    while (1)
    {
        cap.read(tmpMat);
        DataCount = InferEngineSub->CVInferDataSync(tmpMat, MYData, MYDataSize);
        cv::imshow("test", tmpMat);
        for (int r = 0; r < DataCount; r++)
        {
            DetectionIndex = MYData[r * 6];
            std::cout << DetectionIndex << "\n";
        }
        if (DetectionIndex == 1)
        {
            Config = Config + 1;
        }
        else
        {
            Config = 0;
        }
        IsRecoring = Config > 5;
        if (IsRecoring)
        {
            CountDown = 200;
        }
        if (CountDown > 1)
        {
            MyVideo << tmpMat;
            CountDown--;
        }
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    tmpMat.release();
    MyVideo.release();
    cv::destroyAllWindows();
    return 0;
}
