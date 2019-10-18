#include "ComClient.hpp"

void APMClient::CliClient::Client_server()
{
	while (true)
	{
		std::cout << ">>";
		std::cin >> incommingData;

		if (incommingData.compare("dnncamera start") == 0)
		{
			auto thread = std::async(std::launch::async, [] {
				CameraCOM::FramePost StartNCSCamera;
				StartNCSCamera.CameraDNNOutput(0);
				});
		}
		else if (incommingData.compare("dnncamera start") == 0)
		{

		}
	}
}