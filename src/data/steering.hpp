#ifndef STEERING_HPP
#define STEERING_HPP
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

float getGSR(cv::Mat centerBlue, cv::Mat centerYellow, opendlv::proxy::VoltageReading leftVoltage, opendlv::proxy::VoltageReading rightVoltage, bool *isBlueLeft);
float getCvGSR(cv::Mat centerBlue, cv::Mat centerYellow, bool *isBlueLeft);
bool determineConeColors(cv::Mat imgColorSpaceBlue, cv::Mat imgColorSpaceYellow, cv::Rect cent, cv::Rect centerRight, bool *isBlueLeft);
float getIrGSR(opendlv::proxy::VoltageReading leftVoltage, opendlv::proxy::VoltageReading rightVoltage);

#endif // STEERING_HPP