#include "M_Camera.hpp"

cv::Mat CameraCOM::MatDeal::FrameMotionDEBlur(cv::Mat inputFrame)
{
	cv::Mat outputFrame;
	cv::Rect roi = cv::Rect(0, 0, inputFrame.cols & -2, inputFrame.rows & -2);
	cv::Mat Hw, h;
	CalcPSF(h, roi.size(), DeBlur_Config.MODE_LEN, DeBlur_Config.MODE_THETA);
	CalcWnrFilter(h, Hw, 1.0 / double(DeBlur_Config.MODE_THETA));
	inputFrame.convertTo(inputFrame, CV_32F);
	Edgetaper(inputFrame, inputFrame);
	Filter2DFreq(inputFrame(roi), outputFrame, Hw);
	outputFrame.convertTo(outputFrame, CV_8U);
	cv::normalize(outputFrame, outputFrame, 0, 255, cv::NORM_MINMAX);
	return outputFrame;
}

cv::Mat CameraCOM::MatDeal::FrameOFFDEBlur(cv::Mat inputFrame)
{
	cv::Mat outputFrame;
	cv::Rect roi = cv::Rect(0, 0, inputFrame.cols & -2, inputFrame.rows & -2);
	cv::Mat hw, h;
	CalcPSF(h, roi.size(), DeBlur_Config.OFF_Radiu);
	CalcWnrFilter(h, hw, 1.0 / double(5200));
	Filter2DFreq(inputFrame(roi), outputFrame, hw);
	outputFrame.convertTo(outputFrame, CV_8U);
	cv::normalize(outputFrame, inputFrame, 0, 255, cv::NORM_MINMAX);
	return outputFrame;
}

//this is a OFF deblur
void CameraCOM::MatDeal::CalcPSF(cv::Mat &outframe, cv::Size filterSize, int R)
{
	cv::Mat h(filterSize, CV_32F, cv::Scalar(0));
	cv::Point point(filterSize.width / 2, filterSize.height / 2);
	cv::circle(h, point, R, 255, -1, 8);
	cv::Scalar summa = sum(h);
	outframe = h / summa[0];
}

//this is a motion deblur
void CameraCOM::MatDeal::CalcPSF(cv::Mat &outputframe, cv::Size filterSize, int len, double theta)
{
	cv::Mat h(filterSize, CV_32F, cv::Scalar(0));
	cv::Point point(filterSize.width / 2, filterSize.height / 2);
	ellipse(h, point, cv::Size(0, cvRound(float(len) / 2.0)), 90.0 - theta, 0, 360, cv::Scalar(255), cv::FILLED);
	cv::Scalar summa = sum(h);
	outputframe = h / summa[0];
}

void CameraCOM::MatDeal::Fftshift(const cv::Mat &inputFrame, cv::Mat &outputFrame)
{
	outputFrame = inputFrame.clone();
	int cx = outputFrame.cols / 2;
	int cy = outputFrame.rows / 2;
	cv::Mat q0(outputFrame, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(outputFrame, cv::Rect(cx, 0, cx, cy));
	cv::Mat q2(outputFrame, cv::Rect(0, cy, cx, cy));
	cv::Mat q3(outputFrame, cv::Rect(cx, cy, cx, cy));
	cv::Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

void CameraCOM::MatDeal::Filter2DFreq(const cv::Mat &inputFrame, cv::Mat &outputFrame, const cv::Mat &H)
{
	cv::Mat planes[2] = {cv::Mat_<float>(inputFrame.clone()), cv::Mat::zeros(inputFrame.size(), CV_32F)};
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);
	cv::dft(complexI, complexI, cv::DFT_SCALE);
	cv::Mat planesH[2] = {cv::Mat_<float>(H.clone()), cv::Mat::zeros(H.size(), CV_32F)};
	cv::Mat complexH;
	cv::merge(planesH, 2, complexH);
	cv::Mat complexIH;
	cv::mulSpectrums(complexI, complexH, complexIH, 0);
	cv::idft(complexIH, complexIH);
	cv::split(complexIH, planes);
	outputFrame = planes[0];
}

void CameraCOM::MatDeal::CalcWnrFilter(const cv::Mat &input_h_PSF, cv::Mat &output_G, double nsr)
{
	cv::Mat h_PSF_shifted;
	Fftshift(input_h_PSF, h_PSF_shifted);
	cv::Mat planes[2] = {cv::Mat_<float>(h_PSF_shifted.clone()), cv::Mat::zeros(h_PSF_shifted.size(), CV_32F)};
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);
	cv::dft(complexI, complexI);
	cv::split(complexI, planes);
	cv::Mat denom;
	cv::pow(abs(planes[0]), 2, denom);
	denom += nsr;
	cv::divide(planes[0], denom, output_G);
}
//here are motiondeblur function last
void CameraCOM::MatDeal::Edgetaper(const cv::Mat &inputFrame, cv::Mat &outputFrame, double gamma, double Beta)
{
	int Nx = inputFrame.cols;
	int Ny = inputFrame.rows;
	cv::Mat w1(1, Nx, CV_32F, cv::Scalar(0));
	cv::Mat w2(Ny, 1, CV_32F, cv::Scalar(0));
	float *p1 = w1.ptr<float>(0);
	float *p2 = w2.ptr<float>(0);
	float dx = float(2.0 * CV_PI / Nx);
	float x = float(-CV_PI);
	for (int i = 0; i < Nx; i++)
	{
		p1[i] = float(0.5 * (tanh((x + gamma / 2) / Beta) - tanh((x - gamma / 2) / Beta)));
		x += dx;
	}
	float dy = float(2.0 * CV_PI / Ny);
	float y = float(-CV_PI);
	for (int i = 0; i < Ny; i++)
	{
		p2[i] = float(0.5 * (tanh((y + gamma / 2) / Beta) - tanh((y - gamma / 2) / Beta)));
		y += dy;
	}
	cv::Mat w = w2 * w1;
	cv::multiply(inputFrame, w, outputFrame);
}