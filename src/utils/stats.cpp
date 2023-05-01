#include "stats.hpp"

void calculateStats(
    cv::Mat imgCenterBlue,
    cv::Mat imgCenterYellow,
    opendlv::proxy::GroundSteeringRequest gsr,
    bool isBlueLeft)
{

    int hasBlue = 0;
    int hasYellow = 0;
    bool blue = false;
    bool yellow = false;
    bool gsrbool = false;
    static int countYellow = 0;
    static int countBlue = 0;
    static int onlygsrblue = 0;
    static int onlyblue = 0;
    static int onlyyellow = 0;
    static int onlygsryellow = 0;

    hasBlue = cv::countNonZero(imgCenterBlue);

    // this is when the car should turn right
    if (hasBlue > 250)
    {
        blue = true;
    }
    // this is when the car actually turns right
    gsrbool = (isBlueLeft && gsr.groundSteering() < 0) || gsr.groundSteering() < 0;

    if (blue && gsrbool)
    {
        countBlue++;
    }
    if (blue && !gsrbool)
    {
        onlyblue++;
    }
    if (!blue && gsrbool)
    {
        onlygsrblue++;
    }

    std::cout << "Correct blue  : ";
    std::cout << countBlue << std::endl;
    std::cout << "onlyBlue      : ";
    std::cout << onlyblue << std::endl;
    std::cout << "only Gsr right: ";
    std::cout << onlygsrblue << std::endl;
    // std::cout << ourGroundSteering;
    // std::cout << gsr.groundSteering();
    blue = false;
    gsrbool = false;

    hasYellow = 0;
    hasYellow = cv::countNonZero(imgCenterYellow);
    // this is when the car should turn left
    if (hasYellow > 250)
    {
        yellow = true;
    }

    gsrbool = (isBlueLeft && gsr.groundSteering() > 0) || gsr.groundSteering() < 0;

    if (yellow && gsrbool)
    {
        countYellow++;
    }
    if (yellow && !gsrbool)
    {
        onlyyellow++;
    }
    if (!yellow && gsrbool)
    {
        onlygsryellow++;
    }
    std::cout << "Correct yellow: ";
    std::cout << countYellow << std::endl;
    std::cout << "onlyYellow    : ";
    std::cout << onlyblue << std::endl;
    std::cout << "only Gsr left : ";
    std::cout << onlygsryellow << std::endl;

    yellow = false;
    gsrbool = false;
}