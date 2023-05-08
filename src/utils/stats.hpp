#ifndef STATS_HPP
#define STATS_HPP
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>

void calculateStats(
    cv::Mat imgCenterBlue,
    cv::Mat imgCenterYellow,
    opendlv::proxy::GroundSteeringRequest gsr,
    bool isBlueLeft);

void writePixels(float bluePixels, float YellowPixels, float gsr, float calcGsr);

#endif // STATS_HPP