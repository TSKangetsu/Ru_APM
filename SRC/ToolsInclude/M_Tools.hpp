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

	class Socket
	{
	public:
		Socket();
		virtual ~Socket();
		bool create();
		bool bind(const char* ip, const int port);
		bool listen() const;
		bool accept(Socket&) const;
		bool connect(const std::string host, const int port);
		bool send(const std::string) const;
		int recv(std::string&) const;
		void set_non_blocking(const bool);
		bool is_valid() const { return m_sock != -1; }
	private:
		int m_sock;
		sockaddr_in m_addr;
	};
}
