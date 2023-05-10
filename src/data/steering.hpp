#ifndef STEERING_HPP
#define STEERING_HPP
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

float getCvGSR(cv::Mat centerBlue, cv::Mat centerYellow);
bool determineConeColors(cv::Mat imgColorSpaceBlue, cv::Mat imgColorSpaceYellow, cv::Rect cent, cv::Rect centerRight);

#endif // STEERING_HPP