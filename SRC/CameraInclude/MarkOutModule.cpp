#include "M_Camera.hpp"

cv::Mat CameraCOM::MarkOutModule::ColorCut(cv::Mat InputArray, cv::Scalar Settings)
{
	cv::Mat DealTmp;
	cv::Mat ReasultMat;
	cv::Mat Element;

	clock_t begin;
	clock_t end;

	begin = clock();

	Element = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(8, 8),
		cv::Point(2, 2)
	);

	cv::dilate(InputArray, ReasultMat, Element);

	cv::erode(
		ReasultMat,
		ReasultMat,
		Element
	);

	cv::GaussianBlur(
		ReasultMat,
		ReasultMat,
		cv::Size(11, 11),
		0
	);

	cv::inRange(ReasultMat, cv::Scalar(0, 8, 0), cv::Scalar(0, 255, 128), ReasultMat);

	end = clock();
	std::cout << "MatDealTime: " << (int)(end - begin) << "\n";

	return ReasultMat;
}

cv::Point* CameraCOM::MarkOutModule::ImgMarkOut(cv::Mat InputArrayRanged)
{
	cv::Mat contourfound;
	cv::findContours(InputArrayRanged, contourfound, cv::RETR_LIST ,cv::CHAIN_APPROX_NONE);

	std::cout << contourfound.size() << "\n";

	cv::Point* resultLocation = new cv::Point[10];
	return resultLocation;
}