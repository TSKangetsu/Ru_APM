#include "M_Camera.hpp"

CameraCOM::CnnCaculate::CnnCaculate(std::string args1,
									std::string args2,
									int modelType,
									int Backend,
									int Target)
{
	try
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
			break;
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
	}
	catch (const std::exception &e)
	{
		std::cout << "\033[031m[DnnModule]Load net model or protxt failed , info : ";
		std::cerr << e.what();
		std::cout << "\033[0m\n";
		return;
	}
	try
	{
		std::cout << "\033[032m[DnnModule]Setting Target and bankend ........";
		NetInside.setPreferableBackend(Backend);
		NetInside.setPreferableTarget(Target);
		std::cout << "success!\033[0m\n";
	}
	catch (const std::exception &e)
	{
		std::cout << "\033[31m[DnnModule]Are you using raspbian with VPU?";
		std::cerr << e.what();
		std::cout << "\033[0m\n";
	}
}

cv::Mat CameraCOM::CnnCaculate::MatCnn(cv::Mat inputFrame, int widSize, int heiSize, float confidence_threshold)
{
	cv::Mat blobImage = cv::dnn::blobFromImage(inputFrame,
											   1.0,
											   cv::Size(widSize, heiSize),
											   cv::Scalar(100, 100, 100),
											   true, false);

	NetInside.setInput(blobImage);
	cv::Mat outRaw = NetInside.forward();

	std::vector<double> layersTimings;
	double tick_freq = cv::getTickFrequency();
	double time_ms = NetInside.getPerfProfile(layersTimings) / tick_freq * 1000;
	cv::putText(inputFrame, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
				cv::Point(20, 20), 0, 0.5, cv::Scalar(255, 100, 0));

	cv::Mat resultsMat = Base::TransTools::ByteToMat(outRaw.ptr<u_char>(),
													 outRaw.size[2],
													 outRaw.size[3],
													 CV_32F);

	for (int i = 0; i < resultsMat.rows; i++)
	{
		float confidence = resultsMat.at<float>(i, 2);
		if (confidence > confidence_threshold)
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
		}
	}
	return inputFrame;
}

void CameraCOM::CnnCaculate::MatCnnRaspi(int widSize, int heiSize)
{
	std::string pb_model = "/home/pi/TestData/frozen_inference_graph.xml";
	std::string pb_txt = "/home/pi/TestData/frozen_inference_graph.bin";

	cv::dnn::Net net = cv::dnn::readNetFromModelOptimizer(pb_model, pb_txt);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_MYRIAD);

	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 300);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 300);
	cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
	cv::Mat tmpMat;

	while (true)
	{
		cap >> tmpMat;
		cv::Mat blob = cv::dnn::blobFromImage(tmpMat, 1.0, cv::Size(300, 300), cv::Scalar(100, 100, 100), true, false);
		net.setInput(blob);
		cv::Mat outRaw = net.forward();

		std::vector<double> layersTimings;
		double tick_freq = cv::getTickFrequency();
		double time_ms = net.getPerfProfile(layersTimings) / tick_freq * 1000;
		cv::putText(tmpMat, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
					cv::Point(20, 20), 0, 0.5, cv::Scalar(0, 0, 255));

		cv::Mat detectionMat = Base::TransTools::ByteToMat(outRaw.ptr<u_char>(),
														   outRaw.size[2],
														   outRaw.size[3],
														   CV_32F);
		float confidence_threshold = 0.4;
		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);
			if (confidence > confidence_threshold)
			{
				size_t objIndex = (size_t)(detectionMat.at<float>(i, 1));
				float tl_x = detectionMat.at<float>(i, 3) * tmpMat.cols;
				float tl_y = detectionMat.at<float>(i, 4) * tmpMat.rows;
				float br_x = detectionMat.at<float>(i, 5) * tmpMat.cols;
				float br_y = detectionMat.at<float>(i, 6) * tmpMat.rows;

				cv::Rect object_box((int)tl_x, (int)tl_y, (int)(br_x - tl_x), (int)(br_y - tl_y));
				rectangle(tmpMat, object_box, cv::Scalar(255, 0, 255), 1, 8, 0);
				putText(tmpMat,
						cv::format(" confidence %.2f", confidence),
						cv::Point(tl_x - 10, tl_y - 5),
						cv::FONT_HERSHEY_SIMPLEX, 0.7,
						cv::Scalar(255, 0, 0), 2, 8);
			}
		}
		imshow("ins", tmpMat);
		if (cv::waitKey(10) == 'q')
			break;
	}
}