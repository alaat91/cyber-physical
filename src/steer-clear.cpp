#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <cstdlib>

#include "utils/stats.hpp"

float getGSR(cv::Mat centerBlue, cv::Mat centerYellow, bool isBlueLeft);

int main(int argc, char **argv)
{
    int retCode{1};

    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);

    if ((0 == commandlineArguments.count("name")) ||
        (0 == commandlineArguments.count("width")) ||
        (0 == commandlineArguments.count("height")))
    {

        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image and transform it to HSV color space for inspection." << std::endl;
        std::cerr << "Usage: " << argv[0] << " --name=<name of shared memory area> --width=<W> --height=<H>" << std::endl;
        std::cerr << " --name: name of the shared memory area to attach" << std::endl;
        std::cerr << " --width: width of the frame" << std::endl;
        std::cerr << " --height: height of the frame" << std::endl;
        std::cerr << "Example: " << argv[0] << " --name=img.argb --width=640 --height=480" << std::endl;
    }
    else
    {
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};

        bool isSteeringDetermined = false;
        bool isBlueLeft = false;

        std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};

        if (sharedMemory && sharedMemory->valid())
        {
            std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

            // Interface to a running OpenDaVINCI session where network messages are exchanged.
            // The instance od4 allows you to send and receive messages.
            cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

            opendlv::proxy::GroundSteeringRequest gsr;
            std::mutex gsrMutex;
            auto onGroundSteeringRequest = [&gsr, &gsrMutex](cluon::data::Envelope &&env)
            {
                // The envelope data structure provide further details, such as sampleTimePoint as shown in this test case:
                // https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestEnvelopeConverter.cpp#L31-L40
                std::lock_guard<std::mutex> lck(gsrMutex);
                gsr = cluon::extractMessage<opendlv::proxy::GroundSteeringRequest>(std::move(env));
            };

            od4.dataTrigger(opendlv::proxy::GroundSteeringRequest::ID(), onGroundSteeringRequest);

            while (od4.isRunning() && cv::waitKey(10))
            {
                // OpenCV data structure to hold an image.
                cv::Mat img;

                // Wait for a notification of a new frame.
                sharedMemory->wait();

                // Lock the shared memory.
                sharedMemory->lock();
                {
                    // Copy the pixels from the shared memory into our own data structure.
                    cv::Mat wrapped(HEIGHT, WIDTH, CV_8UC4, sharedMemory->data());
                    img = wrapped.clone();
                }
                sharedMemory->unlock();

                cv::Mat imgHSV;
                cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
                cv::Mat imgColorSpaceBlue;
                cv::inRange(imgHSV, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBlue);
                cv::Mat imgColorSpaceYellow;
                cv::inRange(imgHSV, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYellow);
                cv::rectangle(img, cv::Point(180, 250), cv::Point(500, 400), cv::Scalar(0, 0, 255));

                cv::Rect cent(180, 250, 350, 150);
                cv::Mat imgCenterBlue = imgColorSpaceBlue(cent);
                cv::Mat imgCenterYellow = imgColorSpaceYellow(cent);

                if (!isSteeringDetermined)
                {
                    // check for blue on the left half of image
                    cv::Rect leftSide(0, 0, 320, 480);
                    cv::Mat imageLEFT = imgHSV(leftSide);
                    cv::inRange(imageLEFT, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBlue);
                  
                    // check for yellow on the right half of image
                    cv::Rect rightSide(320, 0, 320, 480);
                    cv::Mat imageRIGHT = imgHSV(rightSide);
                    cv::inRange(imageRIGHT, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYellow);

                    int bluePixels = cv::countNonZero(imgColorSpaceBlue);
                    int yellowPixels = cv::countNonZero(imgColorSpaceYellow);
                    if (bluePixels > 30 && yellowPixels > 30)
                    {
                        isBlueLeft = true;
                        isSteeringDetermined = true;
                        std::cout << "Blue is on the left" << std::endl;
                    }
                    else{
                        imageLEFT = imgHSV(leftSide);
                        cv::inRange(imageLEFT, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYellow);
                        imageRIGHT = imgHSV(rightSide);
                        cv::inRange(imageRIGHT, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBlue);
                        bluePixels = cv::countNonZero(imgColorSpaceBlue);
                        yellowPixels = cv::countNonZero(imgColorSpaceYellow);
                        if (bluePixels > 30 && yellowPixels > 30)
                        {
                            isBlueLeft = false;
                            isSteeringDetermined = true;
                            std::cout << "Blue is on the right" << std::endl;
                        }
                    }
                }

                // If you want to access the latest received ground steering, don't forget to lock the mutex:
                {
                    std::lock_guard<std::mutex> lck(gsrMutex);
                    // calculateStats(imgCenterBlue, imgCenterYellow, gsr, isBlueLeft);
                    float g1 = gsr.groundSteering();
                    float g2 = getGSR(imgCenterBlue, imgCenterYellow, isBlueLeft);
                    determineError(g1, g2);
                    writePixels(cv::countNonZero(imgCenterBlue), cv::countNonZero(imgCenterYellow), gsr.groundSteering(), g2);
                }

                cv::imshow("Color-Space Yellow", imgCenterYellow);
                cv::imshow("Color-Space Blue", imgCenterBlue);
            }
        }
        retCode = 0;
    }
    return retCode;
}

float getGSR(cv::Mat centerBlue, cv::Mat centerYellow, bool isBlueLeft)
{
    float gsr = 0;
    int bluePixels = cv::countNonZero(centerBlue);
    int yellowPixels = cv::countNonZero(centerYellow);

    float COLOR_THRESHOLD = 240;
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
    } else if(isBlueLeft){
        gsr -= 0.049;
    } else {
        gsr += 0.049;
    }

    return gsr;
}
