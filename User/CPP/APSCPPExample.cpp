#include "APSUser.hpp"

void APSCPPUser::Setup()
{
    clockTimer = 0;
    RequestDISARM(false);
};

void APSCPPUser::Loop()
{
    GetRTStatus(Status);
    GetRCValues(RCValues);
};

int APSCPPUser::CircleDetect(cv::Mat &IMG, float &x, float &y)
{
    cv::Mat edges;
    cv::cvtColor(IMG, edges, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(edges, edges, cv::Size(5, 5), 2, 2);
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, 1.5, 10, 200, 100, 0, 0);
    int x_Total = 0;
    int y_Total = 0;
    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        x_Total += cvRound(circles[i][0]);
        y_Total += cvRound(circles[i][1]);
        circle(IMG, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        circle(IMG, center, radius, cv::Scalar(155, 50, 255), 3, 8, 0);
    }
    if (circles.size() > 0)
    {
        x = x_Total / circles.size();
        y = y_Total / circles.size();
    }
    return circles.size();
}