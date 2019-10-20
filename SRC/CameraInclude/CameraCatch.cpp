#include "M_Camera.hpp"

CameraCOM::FramePost::FramePost()
{
	JsonConfig::CVConfig Config;
	nlohmann::json data;
	//parse will error on windows mvsc , it just a intellisense problem , not need to do something
	data = nlohmann::json::parse(Config.dataTmp);
	CameraConfig.Camera_Buff = data["CameraConfig"]["Camera_FrameBuff"].get<int>();
	CameraConfig.Camera_Height = data["CameraConfig"]["Camera_Height"].get<int>();
	CameraConfig.Camera_Width = data["CameraConfig"]["Camera_Width"].get<int>();
}

CameraCOM::FramePost::FramePost(int frameBufferCount)
{
	JsonConfig::CVConfig Config;
	nlohmann::json data;
	//parse will error on windows mvsc , it just a intellisense problem , not need to do something
	data = nlohmann::json::parse(Config.dataTmp);
	CameraConfig.Camera_Buff = data["CameraConfig"]["Camera_FrameBuff"].get<int>();
	CameraConfig.Camera_Height = data["CameraConfig"]["Camera_Height"].get<int>();
	CameraConfig.Camera_Width = data["CameraConfig"]["Camera_Width"].get<int>();

	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, CameraConfig.Camera_Height);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, CameraConfig.Camera_Width);
	cap.set(cv::CAP_PROP_BUFFERSIZE, CameraConfig.Camera_Buff);
	cv::Mat capFrameTmp;
	while (Asyncprograssing)
	{
		cap >> capFrameTmp;
		if (!capFrameTmp.empty())
		{
			AsyncCamBuffer.pushFrame(capFrameTmp);
		}
		else
		{
			break;
		}
	}
}


#ifdef linux
int CameraCOM::FramePost::FramePostNet(int startCode)
{
	cv::Mat CatchTMP;
	cv::VideoCapture VideoCatch(startCode);
	Base::Socket PostBind;
	Base::Socket SockSub;
	int imRows, imCols, imSize, imTrueSize, imType;
	unsigned char* imData;

	if (!VideoCatch.isOpened())
	{
		std::cout << "\033[35m[CameraStatus] camera start failed\033[0m\n";
		return -1;
	}
	else
	{
		VideoCatch >> CatchTMP;
		Base::TransTools::MatToByte(CatchTMP, imData, imRows, imCols, imSize, imTrueSize, imType);
		int imInfo[5] = { imRows, imCols, imSize, imTrueSize, imType };
		PostBind.SocketServer(SockSub, 10086, 2);
		if (SockSub.Send(imInfo, 100)) //step 1
		{
			std::cout << "\033[35m[CameraStatus] camera start to post\033[0m\n";
			while (true)
			{
				VideoCatch >> CatchTMP;
				Base::TransTools::MatToByte(CatchTMP, imData, imRows, imCols, imSize, imTrueSize, imType);
				SockSub.Send(imData, imTrueSize); //step 3
			}
		}
		else
		{
			std::cout << "\033[31m[CameraStatus]camera info send failed \033[0m\n";
			return -1;
		}
		return 0;
	}
}
#endif


