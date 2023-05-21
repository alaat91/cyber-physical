#include "steering.hpp"


bool determineConeColors(cv::Mat imgColorSpaceBlue, cv::Mat imgColorSpaceYellow, cv::Rect centerLeft, cv::Rect centerRight, bool *isBlueLeft)
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
        if (bluePixels > 30 && yellowPixels > 30)
        {
            *isBlueLeft = true;
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
                *isBlueLeft = false;
                return true;
            }
        }
    }
    return false;
}

float getGSR(cv::Mat centerBlue, cv::Mat centerYellow, float left_voltage_data, float right_voltage_data, bool *isBlueLeft)
{
    float cv = getCvGSR(centerBlue, centerYellow, isBlueLeft);
    float ir = getIrGSR(left_voltage_data, right_voltage_data);
    if(cv == 0 && ir == -1){
        return *isBlueLeft ? -0.049f : 0.049f;
    } else if(cv != 0){
        return cv;
    } else {
        return ir;
    }

}

float getCvGSR(cv::Mat centerBlue, cv::Mat centerYellow, bool *isBlueLeft)
{
    float gsr = 0;  // Initialize the variable to hold the GSR value.
    int bluePixels = cv::countNonZero(centerBlue);  // Count the number of non-zero pixels in the centerBlue matrix.
    int yellowPixels = cv::countNonZero(centerYellow);  // Count the number of non-zero pixels in the centerYellow matrix.

    float COLOR_THRESHOLD = 600;
    float INPUT_LOWER_BOUND = 0;
    float INPUT_UPPER_BOUND = 1200;
    float OUTPUT_LOWER_BOUND = 0;
    float OUTPUT_UPPER_BOUND = 0.21;

    // Calculate the slope for the GSR calculation.
    float slope = (OUTPUT_UPPER_BOUND - OUTPUT_LOWER_BOUND) / (INPUT_UPPER_BOUND - INPUT_LOWER_BOUND);

    if (isBlueLeft)  // Check if the blue color is on the left side.
    {
        if (bluePixels > COLOR_THRESHOLD)
        {
            // If bluePixels exceeds the color threshold, calculate the GSR value accordingly and subtract from gsr.
            gsr -= (bluePixels * slope) + OUTPUT_LOWER_BOUND;
        }
        else if (yellowPixels > COLOR_THRESHOLD)
        {
            // If bluePixels doesn't exceed the color threshold but yellowPixels does, calculate the GSR value accordingly and add to gsr.
            gsr += (bluePixels * slope) + OUTPUT_LOWER_BOUND;
        }
    }
    else  // If the blue color is on the right side.
    {
        if (bluePixels > COLOR_THRESHOLD)
        {
            // If bluePixels exceeds the color threshold, calculate the GSR value accordingly and add to gsr.
            gsr += (bluePixels * slope) + OUTPUT_LOWER_BOUND;
        }
        else if (yellowPixels > COLOR_THRESHOLD)
        {
            // If bluePixels doesn't exceed the color threshold but yellowPixels does, calculate the GSR value accordingly and subtract from gsr.
            gsr -= (bluePixels * slope) + OUTPUT_LOWER_BOUND;
        }
    }

    return gsr;  // Return the calculated GSR value.
}

// Calculate steeringWheelAngle bases on infrared sensor data
float getIrGSR(float left_voltage_data, float right_voltage_data) {

    if (left_voltage_data >= 0.09f) { // Check if leftVoltage is 0.09 or higher
        return -0.049f; // Set steeringWheelAngle to -0.03 (turn right)
    } else if (right_voltage_data >= 0.04f) { // Check if rightVoltage is 0.09 or higher
        return 0.048f; // Set steeringWheelAngle to 0.03 (turn left)
    }
    return -1; // Set steeringWheelAngle to zero (no turn).
}
