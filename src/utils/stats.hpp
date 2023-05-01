#ifndef STATS_HPP
#define STATS_HPP
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


void calculateStats(
    cv::Mat imgCenterBlue,
    cv::Mat imgCenterYellow,
    opendlv::proxy::GroundSteeringRequest gsr,
    bool isBlueLeft);

#endif // STATS_HPP