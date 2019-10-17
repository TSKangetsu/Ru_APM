#include "ComClient.hpp"

void APMClient::CliClient::Client_server()
{
	while (true)
	{
		while (true)
		{
			std::cout << ">>";
			std::cin >> incommingData;
			std::cout << "\n";
			break;
		}
		if (incommingData.c_str() == "CameraNCSStart")
		{
			std::cout << "NCSCameraStart" << "\n";
			auto thread = std::async(std::launch::async, [] {
				CameraCOM::FramePost StartNCSCamera;
				StartNCSCamera.CameraDNNOutput(0);
			});
		}
	}
}