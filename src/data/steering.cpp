#include "steering.hpp"

static bool isBlueLeft = false;

bool determineConeColors(cv::Mat imgColorSpaceBlue, cv::Mat imgColorSpaceYellow, cv::Rect centerLeft, cv::Rect centerRight)
{
    static bool isSteeringDetermined = false;
    if (!isSteeringDetermined)
    {
        // check for blue on the left half of image
        cv::Rect leftSide(0, 0, 320, 480);
        cv::Mat imageLEFT = imgColorSpaceBlue(leftSide);

        // check for yellow on the right half of image
        cv::Rect rightSide(320, 0, 320, 410);
        cv::Mat imageRIGHT = imgColorSpaceYellow(rightSide);

        int bluePixels = cv::countNonZero(imageLEFT);
        int yellowPixels = cv::countNonZero(imageRIGHT);
        std::cout << bluePixels << std::endl;
        std::cout << yellowPixels << std::endl;
        if (bluePixels > 30 && yellowPixels > 30)
        {
            isBlueLeft = true;
            std::cout << "Blue is on the left" << std::endl;
            return true;
        }
        else
        {
            imageLEFT = imgColorSpaceYellow(leftSide);
            imageRIGHT = imgColorSpaceBlue(rightSide);
            bluePixels = cv::countNonZero(imageRIGHT);
            yellowPixels = cv::countNonZero(imageLEFT);
            if (bluePixels > 30 && yellowPixels > 30)
            {
                isBlueLeft = false;
                std::cout << "Blue is on the right" << std::endl;
                return true;
            }
        }
    }
    return false;
}

float getGSR(cv::Mat centerBlue, cv::Mat centerYellow, opendlv::proxy::VoltageReading leftVoltage, opendlv::proxy::VoltageReading rightVoltage)
{
    float cv = getCvGSR(centerBlue, centerYellow);
    return cv != -1 ? cv : getIrGSR(leftVoltage, rightVoltage);
}

float getCvGSR(cv::Mat centerBlue, cv::Mat centerYellow)
{
    float gsr = 0;
    int bluePixels = cv::countNonZero(centerBlue);
    int yellowPixels = cv::countNonZero(centerYellow);

    float COLOR_THRESHOLD = 220;
    float INPUT_LOWER_BOUND = 0;
    float INPUT_UPPER_BOUND = 1200;

    float OUTPUT_LOWER_BOUND = 0;
    float OUTPUT_UPPER_BOUND = 0.21;

    float slope = (OUTPUT_UPPER_BOUND - OUTPUT_LOWER_BOUND) / (INPUT_UPPER_BOUND - INPUT_LOWER_BOUND);

    if (isBlueLeft && bluePixels > COLOR_THRESHOLD)
    {
        gsr -= (bluePixels * slope) + OUTPUT_LOWER_BOUND;
    }
    else if (!isBlueLeft && bluePixels > COLOR_THRESHOLD)
    {
        gsr += (bluePixels * slope) + OUTPUT_LOWER_BOUND;
    }
    else if (isBlueLeft && yellowPixels > COLOR_THRESHOLD)
    {
        gsr += (bluePixels * slope) + OUTPUT_LOWER_BOUND;
    }
    else if (!isBlueLeft && yellowPixels > COLOR_THRESHOLD)
    {
        gsr -= (bluePixels * slope) + OUTPUT_LOWER_BOUND;
    }
    else
    {
        gsr = -1;
    }

    return gsr;
}

/*
 * Implement basic calculation of steeringWheelAngle.
 * If leftVoltage is 0.01 or higher, set steeringWheelAngle to -0.04.
 * If rightVoltage is 0.01 or higher, set steeringWheelAngle to 0.04.
 * Else, set steeringWheelAngle to zero.
 */
float getIrGSR(opendlv::proxy::VoltageReading leftVoltage, opendlv::proxy::VoltageReading rightVoltage)
{
    if (leftVoltage.voltage() >= 0.089f)
    {
        return -0.049f;
    } else {
        return 0.049f;
    }
};