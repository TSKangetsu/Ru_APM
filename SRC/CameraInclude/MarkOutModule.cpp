#include "M_Camera.hpp"

CameraCOM::MarkOutModule::MarkOutModule()
{
	JsonConfig::CVConfig JsonSet;
	nlohmann::json Data = nlohmann::json::parse(JsonSet.dataTmp);

	auto colorRangeTmp = Data["Mark_Config"]["Color_Range"].get<std::vector<int>>();
	Markout_Args.Color_Range[0] = cv::Scalar(colorRangeTmp[0], colorRangeTmp[1], colorRangeTmp[2]);
	Markout_Args.Color_Range[1] = cv::Scalar(colorRangeTmp[3], colorRangeTmp[4], colorRangeTmp[5]);
}

cv::Mat CameraCOM::MarkOutModule::ColorCut(cv::Mat InputArray)
{
	cv::Mat ReasultMat;
	cv::Mat Element;
	cv::Mat Element2;

	clock_t begin;
	clock_t end;

	begin = clock();

	cv::cvtColor(InputArray, ReasultMat, cv::COLOR_BGR2HSV);

	cv::inRange(ReasultMat, Markout_Args.Color_Range[0], Markout_Args.Color_Range[1], ReasultMat);

	Element = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(2, 2),
		cv::Point(1, 1)
	);

	Element2 = cv::getStructuringElement(
		cv::MORPH_RECT,
		cv::Size(40, 40),
		cv::Point(1, 1)
	);

	cv::erode(
		ReasultMat,
		ReasultMat,
		Element
	);

	cv::dilate(ReasultMat, ReasultMat, Element2);
	end = clock();

#define DEBUG

#ifdef DEBUG
	std::cout << "MatDealTime :" << (int)(end - begin) << "\n";
#endif

	return ReasultMat;
}

int CameraCOM::MarkOutModule::ImgMarkOut(cv::Mat InputArrayRanged)
{
	std::vector<std::vector<cv::Point>> contourfound;
	cv::findContours(InputArrayRanged, contourfound, cv::RETR_CCOMP, cv::CHAIN_APPROX_TC89_KCOS);
	return contourfound.size();
}