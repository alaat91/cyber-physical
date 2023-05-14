#ifndef STEERING_HPP
#define STEERING_HPP
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

float getGSR(cv::Mat centerBlue, cv::Mat centerYellow, float left_voltage_data, float right_voltage_data);
float getCvGSR(cv::Mat centerBlue, cv::Mat centerYellow);
bool determineConeColors(cv::Mat imgColorSpaceBlue, cv::Mat imgColorSpaceYellow, cv::Rect cent, cv::Rect centerRight);
float getIrGSR(float left_voltage_data, float right_voltage_data);

#endif // STEERING_HPP