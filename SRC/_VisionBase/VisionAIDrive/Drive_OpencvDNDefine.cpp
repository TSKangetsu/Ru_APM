#include "Drive_OpenCVDN.hpp"

CVInferEngine::CVInferEngine(CVInferConfig &config)
{
    InferConfig = config;
    try
    {
        switch (InferConfig.Model_Type)
        {
        case 0:
            InferEngine = cv::dnn::readNet(InferConfig.File_args1, InferConfig.File_args2);
            break;
        case 1:
            InferEngine = cv::dnn::readNetFromModelOptimizer(InferConfig.File_args1, InferConfig.File_args2);
            break;
        case 2:
            InferEngine = cv::dnn::readNetFromTensorflow(InferConfig.File_args1, InferConfig.File_args2);
            break;
        case 3:
            InferEngine = cv::dnn::readNetFromCaffe(InferConfig.File_args1, InferConfig.File_args2);
            break;
        case 4:
            InferEngine = cv::dnn::readNetFromDarknet(InferConfig.File_args1, InferConfig.File_args2);
            break;
        }
    }
    catch (const std::exception &e)
    {
#ifdef DEBUG
        std::cout << "\033[031m[DnnModule]Load net model or protxt failed , info : ";
        std::cerr << e.what();
        std::cout << "\033[0m\n";
#endif
        return;
    }
    try
    {
#ifdef DEBUG
        std::cout << "\033[032m[DnnModule]Setting Target and bankend ........";
#endif
        InferEngine.setPreferableBackend(InferConfig.Preferable_Backend);
        InferEngine.setPreferableTarget(InferConfig.Preferable_Target);
#ifdef DEBUG
        std::cout << "success!\033[0m\n";
#endif
    }
    catch (const std::exception &e)
    {
#ifdef DEBUG
        std::cout << "\033[31m[DnnModule]Are you using raspbian with VPU?";
        std::cerr << e.what();
        std::cout << "\033[0m\n";
#endif
    }
}

cv::Mat CVInferEngine::CVInferMatSync(cv::Mat inputFrame)
{
    cv::Mat blobImage = cv::dnn::blobFromImage(inputFrame,
                                               1.0,
                                               cv::Size(InferConfig.Blob_Size[0], InferConfig.Blob_Size[1]),
                                               cv::Scalar(InferConfig.Blob_Scalar[0], InferConfig.Blob_Scalar[1], InferConfig.Blob_Scalar[2]),
                                               true, false);
    InferEngine.setInput(blobImage);
    cv::Mat outRaw = InferEngine.forward();

    if (InferConfig.Enable_FPS_Detect)
    {
        std::vector<double> layersTimings;
        double tick_freq = cv::getTickFrequency();
        double time_ms = InferEngine.getPerfProfile(layersTimings) / tick_freq * 1000;
        cv::putText(inputFrame, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
                    cv::Point(20, 20), 0, 0.5, cv::Scalar(255, 100, 0));
    }

    cv::Mat resultsMat = cv::Mat(outRaw.size[2], outRaw.size[3], CV_32F, outRaw.ptr<unsigned char>());
    for (int i = 0; i < resultsMat.rows; i++)
    {
        float confidence = resultsMat.at<float>(i, 2);
        if (confidence > InferConfig.Confidence_Threshold)
        {
            size_t objIndex = (size_t)(resultsMat.at<float>(i, 1));
            float tl_x = resultsMat.at<float>(i, 3) * inputFrame.cols;
            float tl_y = resultsMat.at<float>(i, 4) * inputFrame.rows;
            float br_x = resultsMat.at<float>(i, 5) * inputFrame.cols;
            float br_y = resultsMat.at<float>(i, 6) * inputFrame.rows;

            cv::Rect object_box((int)tl_x, (int)tl_y, (int)(br_x - tl_x), (int)(br_y - tl_y));
            cv::rectangle(inputFrame, object_box, cv::Scalar(255, 0, 255), 1, 8, 0);
            putText(inputFrame,
                    cv::format(" confidence %.2f", confidence),
                    cv::Point(tl_x - 10, tl_y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(255, 0, 0), 2, 8);
            putText(inputFrame,
                    cv::format(" Index %.2f", objIndex),
                    cv::Point(tl_x - 10, tl_y - 25),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(255, 0, 0), 2, 8);
        }
    }
    return inputFrame;
}

int CVInferEngine::CVInferDataSync(cv::Mat inputFrame, float *Data, int dataSize)
{
    cv::Mat blobImage = cv::dnn::blobFromImage(inputFrame,
                                               1.0,
                                               cv::Size(InferConfig.Blob_Size[0], InferConfig.Blob_Size[1]),
                                               cv::Scalar(InferConfig.Blob_Scalar[0], InferConfig.Blob_Scalar[1], InferConfig.Blob_Scalar[2]),
                                               true, false);

    InferEngine.setInput(blobImage);
    cv::Mat outRaw = InferEngine.forward();

    // if (InferConfig.Enable_FPS_Detect)
    // {
    //     std::vector<double> layersTimings;
    //     double tick_freq = cv::getTickFrequency();
    //     double time_ms = InferEngine.getPerfProfile(layersTimings) / tick_freq * 1000;
    //     cv::putText(inputFrame, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
    //                 cv::Point(20, 20), 0, 0.5, cv::Scalar(255, 100, 0));
    // }

    cv::Mat resultsMat = cv::Mat(outRaw.size[2], outRaw.size[3], CV_32F, outRaw.ptr<unsigned char>());
    int DataCount = 0;
    for (int i = 0; i < resultsMat.rows; i++)
    {
        float confidence = resultsMat.at<float>(i, 2);
        if (confidence > InferConfig.Confidence_Threshold)
        {
            if (DataCount > dataSize)
                break;
            int set = i * 6;
            Data[set] = resultsMat.at<float>(i, 1);
            Data[set + 1] = resultsMat.at<float>(i, 2) * 100;
            Data[set + 2] = resultsMat.at<float>(i, 3);
            Data[set + 3] = resultsMat.at<float>(i, 4);
            Data[set + 4] = resultsMat.at<float>(i, 5);
            Data[set + 5] = resultsMat.at<float>(i, 6);
            DataCount++;
        }
    }
    return DataCount;
}

cv::Mat CVInferEngine::CVINferMatAsyncBlock(int CAM)
{
    cv::namedWindow("AsyncDnn", cv::WINDOW_NORMAL);
    cv::setWindowProperty("AsyncDnn", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::VideoCapture cap(CAM);
    FrameBuffer<cv::Mat> camBuffer;
    cv::Mat capFrameTmp;
    bool prograssing = true;
    std::thread camThread([&]() {
        while (prograssing)
        {
            cap >> capFrameTmp;
            if (!capFrameTmp.empty())
            {
                camBuffer.pushFrame(capFrameTmp);
            }
            else
            {
                break;
            }
        }
    });

    FrameBuffer<cv::Mat> forwardBuffer;
    FrameBuffer<cv::Mat> campreBuffer;
    std::thread forwardThread([&]() {
        cv::Mat forwardMat;
        std::queue<cv::AsyncArray> asyncForwardingBuffer;
        cv::Mat frameTmp;
        while (true)
        {
            if (camBuffer.empty())
            {
                continue;
            }
            else
            {
                frameTmp = camBuffer.getFrame();
                if (asyncForwardingBuffer.size() == 3)
                    frameTmp = cv::Mat();
            }

            if (!frameTmp.empty())
            {
                cv::Mat blobImage = cv::dnn::blobFromImage(frameTmp,
                                                           1.0,
                                                           cv::Size(InferConfig.Blob_Size[0], InferConfig.Blob_Size[1]),
                                                           cv::Scalar(InferConfig.Blob_Scalar[0], InferConfig.Blob_Scalar[1], InferConfig.Blob_Scalar[2]),
                                                           true, false);
                InferEngine.setInput(blobImage);
                campreBuffer.push(frameTmp);
                asyncForwardingBuffer.push(InferEngine.forwardAsync());
            }

            while (!asyncForwardingBuffer.empty() && asyncForwardingBuffer.front().wait_for(std::chrono::seconds(0)))
            {
                cv::AsyncArray outRawAsync = asyncForwardingBuffer.front();
                asyncForwardingBuffer.pop();
                cv::Mat outRaw;
                outRawAsync.get(outRaw);
                forwardBuffer.push(outRaw);
            }
        }
    });

    while (true)
    {
        if (forwardBuffer.empty())
        {
            continue;
        }
        else
        {
            cv::Mat showTmp = forwardBuffer.getFrame();
            cv::Mat resultsMat = cv::Mat(showTmp.size[2], showTmp.size[3], CV_32F, showTmp.ptr<unsigned char>());
            cv::Mat inputFrame = campreBuffer.getFrame();

            for (int i = 0; i < resultsMat.rows; i++)
            {
                float confidence = resultsMat.at<float>(i, 2);
                if (confidence > InferConfig.Confidence_Threshold)
                {
                    float objIndex = resultsMat.at<float>(i, 1);
                    float tl_x = resultsMat.at<float>(i, 3) * inputFrame.cols;
                    float tl_y = resultsMat.at<float>(i, 4) * inputFrame.rows;
                    float br_x = resultsMat.at<float>(i, 5) * inputFrame.cols;
                    float br_y = resultsMat.at<float>(i, 6) * inputFrame.rows;

                    cv::Rect object_box((int)tl_x, (int)tl_y, (int)(br_x - tl_x), (int)(br_y - tl_y));
                    cv::rectangle(inputFrame, object_box, cv::Scalar(255, 0, 255), 1, 8, 0);
                    putText(inputFrame,
                            cv::format(" confidence %.2f", confidence),
                            cv::Point(tl_x - 10, tl_y - 5),
                            cv::FONT_HERSHEY_SIMPLEX, 0.7,
                            cv::Scalar(255, 0, 0), 2, 8);

                    putText(inputFrame,
                            cv::format(" Index %.2f", objIndex),
                            cv::Point(tl_x - 10, tl_y - 20),
                            cv::FONT_HERSHEY_SIMPLEX, 0.7,
                            cv::Scalar(255, 0, 0), 2, 8);

                    std::string label = cv::format("Camera: %.2f FPS", camBuffer.getDecFPS());
                    cv::putText(inputFrame, label, cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));

                    label = cv::format("Network: %.2f FPS", camBuffer.getDecFPS());
                    cv::putText(inputFrame, label, cv::Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));

                    label = cv::format("Skipped frames: %d", camBuffer.frameCount - forwardBuffer.frameCount);
                    cv::putText(inputFrame, label, cv::Point(0, 45), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));
                }
            }

            cv::imshow("AsyncDnn", inputFrame);
            if (cv::waitKey(10) == 'q')
                break;
        }
    }
    prograssing = false;
}
