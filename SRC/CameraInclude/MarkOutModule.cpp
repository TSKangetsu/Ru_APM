#include "M_Camera.hpp"

CameraCOM::MarkOutModule::MarkOutModule(int modeType)
{
	JsonConfig::CVConfig JsonSet;
	nlohmann::json Data = nlohmann::json::parse(JsonSet.dataTmp);

	auto colorRangeTmp = Data["Color_Range"].get<std::vector<int>>();
	Markout_Args.Color_Range[0] = cv::Scalar(colorRangeTmp[0], colorRangeTmp[1], colorRangeTmp[2]);
	Markout_Args.Color_Range[1] = cv::Scalar(colorRangeTmp[3], colorRangeTmp[4], colorRangeTmp[5]);
}

cv::Mat CameraCOM::MarkOutModule::ColorCut(cv::Mat InputArray, cv::Scalar Settings)
{
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

	cv::erode(
		InputArray,
		ReasultMat,
		Element
	);

	cv::inRange(ReasultMat, Markout_Args.Color_Range[0], Markout_Args.Color_Range[1], ReasultMat);

	cv::erode(
		ReasultMat,
		ReasultMat,
		Element
	);

	cv::dilate(ReasultMat, ReasultMat, Element);

	end = clock();

	std::cout << "MatDealTime: " << (int)(end - begin) << "\n";
	cv::imshow("test", ReasultMat);
	cv::waitKey();
	return ReasultMat;
}

cv::Point* CameraCOM::MarkOutModule::ImgMarkOut(cv::Mat InputArrayRanged)
{
	cv::Mat contourfound;
	cv::findContours(InputArrayRanged, contourfound, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	std::cout << contourfound.size() << "\n";

	cv::Point* resultLocation = new cv::Point[10];
	return resultLocation;
}