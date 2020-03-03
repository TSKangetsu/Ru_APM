#pragma once
#ifndef MTOOLS_H
#define MTOOLS_H
#endif
#include <sys/types.h>
#include <string>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#ifdef linux
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
#ifdef windows
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#endif

namespace Base
{
	class TransTools
	{
	public:
		static unsigned char* MatToByte(cv::Mat imFrame,
			unsigned char*& imData,
			int& imRows,
			int& imCols,
			int& imSize,
			int& imTrueSize,
			int& imType);

		static cv::Mat ByteToMat(unsigned char* imByte,
			int imRows,
			int imCols,
			int imType);

		static unsigned char* StringToByte(std::string strData);
		static std::string ByteToString(unsigned char* byteData);
	};

	class Matcom
	{
	public:
		static int ConvertSaveImg(std::string imName, cv::Mat imageData);
		static cv::Mat ImageGet(std::string imdataName);
	};

	class ConfigSetter
	{
	public:
		static bool CreateConfig(std::string fileName, std::string data);
		static bool ReadConfig(std::string fileName, std::string& data);
	};
} // namespace Base
