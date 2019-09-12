#include "M_Camera.hpp"

CameraCOM::CnnCaculate::CnnCaculate(std::string args1, std::string args2, int modelType, float confidence, int PreferableBackend, int PreferableTarget)
{
    switch (modelType)
    {
    case 0:
        NetInside = cv::dnn::readNet(args1, args2);
        std::cout << "\033[032m[DnnModule]NET custom comfirm\033[0m\n";
        break;
    case 1:
        NetInside = cv::dnn::readNetFromModelOptimizer(args1, args2);
        std::cout << "\033[032m[DnnModule]NET custom for openvino comfirm\033[0m\n";
    case 2:
        NetInside = cv::dnn::readNetFromTensorflow(args1, args2);
        std::cout << "\033[032m[DnnModule]NET tensorflow comfirm\033[0m\n";
        break;
    case 3:
        NetInside = cv::dnn::readNetFromCaffe(args1, args2);
        std::cout << "\033[032m[DnnModule]NET Caffe comfirm\033[0m\n";
        break;
    case 4:
        NetInside = cv::dnn::readNetFromDarknet(args1, args2);
        std::cout << "\033[032m[DnnModule]NET Darknet comfirm\033[0m\n";
        break;
    }
    NetInside.setPreferableBackend(PreferableBackend);
    NetInside.setPreferableTarget(PreferableTarget);
    confidence_threshold = confidence;
}

cv::Mat CameraCOM::CnnCaculate::MatCnn(cv::Mat inputFrame, int widSize, int heiSize)
{
    cv::Mat blobImage = cv::dnn::blobFromImage(inputFrame, 1.0, cv::Size(widSize, heiSize), cv::Scalar(100, 100, 100), true, false);
    NetInside.setInput(blobImage);
    cv::Mat outRaw = NetInside.forward();

    std::vector<double> layersTimings;
    double tick_freq = cv::getTickFrequency();
    double time_ms = NetInside.getPerfProfile(layersTimings) / tick_freq * 1000;
    cv::putText(inputFrame, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
                cv::Point(20, 20), 0, 0.5, cv::Scalar(0, 0, 255));

    cv::Mat resultsMat = Base::TransTools::ByteToMat(outRaw.ptr<u_char>(), outRaw.size[2], outRaw.size[3], CV_32F);
    for (int i = 0; i < resultsMat.rows; i++)
    {
        float confidence = resultsMat.at<float>(i, 2);
        if (confidence > confidence_threshold)
        {
            objIndex = (size_t)(resultsMat.at<float>(i, 1));
            tl_x = resultsMat.at<float>(i, 3) * inputFrame.cols;
            tl_y = resultsMat.at<float>(i, 4) * inputFrame.rows;
            br_x = resultsMat.at<float>(i, 5) * inputFrame.cols;
            br_y = resultsMat.at<float>(i, 6) * inputFrame.rows;
            cv::Rect object_box((int)tl_x, (int)tl_y, (int)(br_x - tl_x), (int)(br_y - tl_y));
            cv::rectangle(inputFrame, object_box, cv::Scalar(255, 0, 255), 1, 8, 0);
            putText(inputFrame, cv::format(" confidence %.2f", confidence), cv::Point(tl_x - 10, tl_y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 0), 2, 8);
        }
    }
    return inputFrame;
}