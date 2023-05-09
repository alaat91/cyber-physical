#include "stats.hpp"

void calculateStats(
    cv::Mat imgCenterBlue,
    cv::Mat imgCenterYellow,
    opendlv::proxy::GroundSteeringRequest gsr,
    bool isBlueLeft)
{

    int bluePixelAmount = 0;
    int yellowPixelAmount = 0;
    bool blue = false;
    bool yellow = false;
    bool gsrbool = false;
    static int countYellow = 0;
    static int countBlue = 0;
    static int onlygsrblue = 0;
    static int onlyblue = 0;
    static int onlyyellow = 0;
    static int onlygsryellow = 0;
    static int emptyBlue = 0;
    static int emptyYellow = 0;

    bluePixelAmount = cv::countNonZero(imgCenterBlue);
    // this is when the car should turn right
    if (bluePixelAmount > 250)
    {
        blue = true;
    }
    // this is when the car actually turns right
    if (isBlueLeft)
    {
        if (gsr.groundSteering() < 0)
        {
            gsrbool = true;
        }
    }
    else
    {
        if (gsr.groundSteering() > 0)
        {
            gsrbool = true;
        }
    }
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
    if (!blue && !gsrbool)
    {
        emptyBlue++;
    }
    // Our algorithm turns the car when it should
    std::cout << "Correct blue  : ";
    std::cout << countBlue << std::endl;
    // Our algorithm turns the car when it shouldn’t
    std::cout << "onlyBlue      : ";
    std::cout << onlyblue << std::endl;
    // Our algorithm doesn’t turn the car when it should
    std::cout << "only Gsr right: ";
    std::cout << onlygsrblue << std::endl;
    // Our algorithm doesn’t turn the car, and it shouldn’t
    std::cout << "Neither output: ";
    std::cout << emptyBlue << std::endl;

    yellowPixelAmount = cv::countNonZero(imgCenterYellow);

    // this is when the car should turn left
    if (yellowPixelAmount > 250)
    {
        yellow = true;
    }

    gsrbool = false;

    if (isBlueLeft)
    {
        if (gsr.groundSteering() > 0)
        {
            gsrbool = true;
        }
    }
    else
    {
        if (gsr.groundSteering() < 0)
        {
            gsrbool = true;
        }
    }

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
    if (!yellow && !gsrbool)
    {
        emptyYellow++;
    }
    // Our algorithm turns the car when it should
    std::cout << "Correct yellow: ";
    std::cout << countYellow << std::endl;
    // Our algorithm turns the car when it shouldn’t
    std::cout << "onlyYellow    : ";
    std::cout << onlyyellow << std::endl;
    // Our algorithm doesn’t turn the car when it should
    std::cout << "only Gsr left : ";
    std::cout << onlygsryellow << std::endl;
    // Our algorithm doesn’t turn the car, and it shouldn’t
    std::cout << "Neither output: ";
    std::cout << emptyYellow << std::endl;

    yellow = false;
    gsrbool = false;
}

void writePixels(float bluePixels, float YellowPixels, float gsr, float calcGsr)
{
    std::ofstream myfile;
    myfile.open("/host/res.csv", std::ios_base::app);
    myfile << bluePixels << "," << YellowPixels << "," << gsr << "," << calcGsr << ";\n";
    myfile.close();
}

void determineError(float g1, float g2)
{
    static float totalFrames = 0;
    static float correctFrames = 0;

    float e = ((fabs(g1 - g2)) / fabs(g1)) * 100;

    if (g1 == 0)
    {
        e = fabs(g1 - g2);
    }
    std::cout << "Original Steering: " << g1 << std::endl;
    std::cout << "Ground Steering: " << g2 << std::endl;
    std::cout << "Error: " << e << std::endl;

    if (g1 != 0 && e <= 30)
    {
        correctFrames++;
    }
    else if (g1 == 0 && fabs(g1 - g2) <= 0.05)
    {
        correctFrames++;
    }
    totalFrames++;
    std::cout << "Frame Stats:" << correctFrames << "/" << totalFrames << "\t" << ((correctFrames / totalFrames) * 100) << " %" << std::endl;
}