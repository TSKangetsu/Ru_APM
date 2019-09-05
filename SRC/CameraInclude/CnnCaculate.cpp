#include "M_Camera.hpp"

CameraCOM::CnnCaculate::CnnCaculate(std::string pbFile, std::string pbtxtFile, int modelType, int PreferableBackend, int PreferableTarget, float confidence, std::string config)
{
    switch (modelType)
    {
    case 0:
        NetInside = cv::dnn::readNetFromTensorflow(pbFile, pbtxtFile);
        std::cout << "\033[032m[DnnModule]NET tensorflow comfirm\033[0m\n";
        break;
    case 1:
        NetInside = cv::dnn::readNetFromCaffe(pbFile, pbtxtFile);
        std::cout << "\033[032m[DnnModule]NET Caffe comfirm\033[0m\n";
        break;
    case 2:
        NetInside = cv::dnn::readNetFromDarknet(pbFile, pbtxtFile);
        std::cout << "\033[032m[DnnModule]NET Darknet comfirm\033[0m\n";
        break;
    case 3:
        NetInside = cv::dnn::readNet(pbFile, config, pbtxtFile);
        std::cout << "\033[032m[DnnModule]NET custom comfirm\033[0m\n";
        break;
    }
    NetInside.setPreferableBackend(PreferableBackend);
    NetInside.setPreferableTarget(PreferableTarget);
    deOut_names.push_back("detection_out_final");
    deOut_names.push_back("detection_masks");
    confidence_threshold = confidence;
    cv::setNumThreads(12);
}

cv::Mat CameraCOM::CnnCaculate::MatCnn(cv::Mat inputFrame, int widSize, int heiSize)
{
    blobImage = cv::dnn::blobFromImage(inputFrame, 1.0, cv::Size(widSize, heiSize), cv::Scalar(0, 0, 0), true, false);
    NetInside.setInput(blobImage);

    NetInside.forward(deOut, deOut_names);
    std::vector<double> layersTimings;
    int time = NetInside.getPerfProfile(layersTimings);
    std::cout << "\033[037m"
              << "detect spend:" << std::setprecision(4) << time << "\033[0m"
              << "\n";

    itmpMat = deOut[0];
    resultsMat = Base::TransTools::ByteToMat(itmpMat.ptr<u_char>(), itmpMat.size[2], itmpMat.size[3], CV_32F);

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