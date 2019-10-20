#include "M_Camera.hpp"

CameraCOM::DnnModule::DnnModule()
{
	JsonConfig::CVConfig Config;
	nlohmann::json data;
	//parse will error on windows mvsc , it just a intellisense problem , not need to do something
	data = nlohmann::json::parse(Config.dataTmp);

	CV_Config.Camera_Buff = data["CameraConfig"]["Camera_FrameBuff"].get<int>();
	CV_Config.Camera_Height = data["CameraConfig"]["Camera_Height"].get<int>();
	CV_Config.Camera_Width = data["CameraConfig"]["Camera_Width"].get<int>();

	auto scalarTmp = data["DnnConfig"]["Blob_Scalar"].get<std::vector<short>>();
	CV_Config.Blob_Scalar[0] = scalarTmp[0];
	CV_Config.Blob_Scalar[1] = scalarTmp[1];
	CV_Config.Blob_Scalar[2] = scalarTmp[2];

	auto sizeTmp = data["DnnConfig"]["Blob_Size"].get<std::vector<short>>();
	CV_Config.Blob_Size[0] = sizeTmp[0];
	CV_Config.Blob_Size[1] = sizeTmp[1];

	CV_Config.confidence_threshold = data["DnnConfig"]["Confidence"].get<float>();
	CV_Config.Model_Type = data["DnnConfig"]["Dnn_Model"].get<int>();
	CV_Config.Preferable_Backend = data["DnnConfig"]["PreferableBackend"].get<int>();
	CV_Config.Preferable_Target = data["DnnConfig"]["PreferableTarget"].get<int>();
	CV_Config.File_args1 = data["DnnConfig"]["ModelDir"]["args1"].get<std::string>();
	CV_Config.File_args2 = data["DnnConfig"]["ModelDir"]["args2"].get<std::string>();
	//
	DnnModuleSet();
}

void CameraCOM::DnnModule::DnnModuleSet()
{
	try
	{
		switch (CV_Config.Model_Type)
		{
		case 0:
			NetInside = cv::dnn::readNet(CV_Config.File_args1, CV_Config.File_args2);
			std::cout << "\033[032m[DnnModule]NET custom comfirm\033[0m\n";
			break;
		case 1:
			NetInside = cv::dnn::readNetFromModelOptimizer(CV_Config.File_args1, CV_Config.File_args2);
			std::cout << "\033[032m[DnnModule]NET custom for openvino comfirm\033[0m\n";
			break;
		case 2:
			NetInside = cv::dnn::readNetFromTensorflow(CV_Config.File_args1, CV_Config.File_args2);
			std::cout << "\033[032m[DnnModule]NET tensorflow comfirm\033[0m\n";
			break;
		case 3:
			NetInside = cv::dnn::readNetFromCaffe(CV_Config.File_args1, CV_Config.File_args2);
			std::cout << "\033[032m[DnnModule]NET Caffe comfirm\033[0m\n";
			break;
		case 4:
			NetInside = cv::dnn::readNetFromDarknet(CV_Config.File_args1, CV_Config.File_args2);
			std::cout << "\033[032m[DnnModule]NET Darknet comfirm\033[0m\n";
			break;
		}
	}
	catch (const std::exception & e)
	{
		std::cout << "\033[031m[DnnModule]Load net model or protxt failed , info : ";
		std::cerr << e.what();
		std::cout << "\033[0m\n";
		return;
	}
	try
	{
		std::cout << "\033[032m[DnnModule]Setting Target and bankend ........";
		NetInside.setPreferableBackend(CV_Config.Preferable_Backend);
		NetInside.setPreferableTarget(CV_Config.Preferable_Target);
		std::cout << "success!\033[0m\n";
	}
	catch (const std::exception & e)
	{
		std::cout << "\033[31m[DnnModule]Are you using raspbian with VPU?";
		std::cerr << e.what();
		std::cout << "\033[0m\n";
	}
}


int CameraCOM::DnnModule::MatDnnDeal(cv::Mat inputFrame, float*& Data)
{
	cv::Mat blobImage = cv::dnn::blobFromImage(inputFrame,
		1.0,
		cv::Size(CV_Config.Blob_Size[0], CV_Config.Blob_Size[1]),
		cv::Scalar(CV_Config.Blob_Scalar[0], CV_Config.Blob_Scalar[1], CV_Config.Blob_Scalar[2]),
		true, false);

	NetInside.setInput(blobImage);
	cv::Mat outRaw = NetInside.forward();

	if (CV_Config.Enable_FPS_Detect)
	{
		std::vector<double> layersTimings;
		double tick_freq = cv::getTickFrequency();
		double time_ms = NetInside.getPerfProfile(layersTimings) / tick_freq * 1000;
		cv::putText(inputFrame, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
			cv::Point(20, 20), 0, 0.5, cv::Scalar(255, 100, 0));
	}

	cv::Mat resultsMat = Base::TransTools::ByteToMat(outRaw.ptr<unsigned char>(),
		outRaw.size[2],
		outRaw.size[3],
		CV_32F);

	Data = new  float[resultsMat.rows * 5 + 1];

	for (int i = 0; i < resultsMat.rows; i++)
	{
		float confidence = resultsMat.at<float>(i, 2);
		if (confidence > CV_Config.confidence_threshold)
		{
			int set = i * 5;
			Data[set] = resultsMat.at<float>(i, 1);
			Data[set + 1] = resultsMat.at<float>(i, 3);
			Data[set + 2] = resultsMat.at<float>(i, 4);
			Data[set + 3] = resultsMat.at<float>(i, 5);
			Data[set + 4] = resultsMat.at<float>(i, 6);

		}
	}
	return 0;
}



cv::Mat CameraCOM::DnnModule::MatDnnDeal(cv::Mat inputFrame)
{
	cv::Mat blobImage = cv::dnn::blobFromImage(inputFrame,
		1.0,
		cv::Size(CV_Config.Blob_Size[0], CV_Config.Blob_Size[1]),
		cv::Scalar(CV_Config.Blob_Scalar[0], CV_Config.Blob_Scalar[1], CV_Config.Blob_Scalar[2]),
		true, false);

	NetInside.setInput(blobImage);
	cv::Mat outRaw = NetInside.forward();

	if (CV_Config.Enable_FPS_Detect)
	{
		std::vector<double> layersTimings;
		double tick_freq = cv::getTickFrequency();
		double time_ms = NetInside.getPerfProfile(layersTimings) / tick_freq * 1000;
		cv::putText(inputFrame, cv::format("FPS: %.2f ; time: %.2f ms", 1000.f / time_ms, time_ms),
			cv::Point(20, 20), 0, 0.5, cv::Scalar(255, 100, 0));
	}

	cv::Mat resultsMat = Base::TransTools::ByteToMat(outRaw.ptr<unsigned char>(),
		outRaw.size[2],
		outRaw.size[3],
		CV_32F);

	for (int i = 0; i < resultsMat.rows; i++)
	{
		float confidence = resultsMat.at<float>(i, 2);
		if (confidence > CV_Config.confidence_threshold)
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

//testbed_function
void CameraCOM::DnnModule::AsyncMatDnnDeal()
{
	cv::namedWindow("AsyncDnn", cv::WINDOW_NORMAL);
	cv::setWindowProperty("AsyncDnn", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, CV_Config.Camera_Height);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, CV_Config.Camera_Width);
	cap.set(cv::CAP_PROP_BUFFERSIZE, CV_Config.Camera_Buff);
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
					cv::Size(CV_Config.Blob_Size[0], CV_Config.Blob_Size[1]),
					cv::Scalar(CV_Config.Blob_Scalar[0], CV_Config.Blob_Scalar[1], CV_Config.Blob_Scalar[2]),
					true, false);
				NetInside.setInput(blobImage);
				campreBuffer.push(frameTmp);
				asyncForwardingBuffer.push(NetInside.forwardAsync());
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
			cv::Mat resultsMat = Base::TransTools::ByteToMat(showTmp.ptr<unsigned char>(),
				showTmp.size[2],
				showTmp.size[3],
				CV_32F);
			cv::Mat inputFrame = campreBuffer.getFrame();

			for (int i = 0; i < resultsMat.rows; i++)
			{
				float confidence = resultsMat.at<float>(i, 2);
				if (confidence > CV_Config.confidence_threshold)
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