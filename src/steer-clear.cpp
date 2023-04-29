#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>

bool isSteeringDetermined = false;

int main(int argc, char **argv)
{
    int retCode{1};

    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);

    if ((0 == commandlineArguments.count("name")) ||
        (0 == commandlineArguments.count("width")) ||
        (0 == commandlineArguments.count("height"))) {

        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image and transform it to HSV color space for inspection." << std::endl;
        std::cerr << "Usage: " << argv[0] << " --name=<name of shared memory area> --width=<W> --height=<H>" << std::endl;
        std::cerr << " --name: name of the shared memory area to attach" << std::endl;
        std::cerr << " --width: width of the frame" << std::endl;
        std::cerr << " --height: height of the frame" << std::endl;
        std::cerr << "Example: " << argv[0] << " --name=img.argb --width=640 --height=480" << std::endl;
    }
    else {
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
        int hasBlue = 0;
        int hasYellow = 0;
        bool blue = false;
        bool yellow = false;
        bool gsrbool = false;
        bool isBLueLeft = false;
        int countYellow = 0;
        int countBlue = 0;
        int onlygsrblue = 0;
        int onlyblue = 0;
        int onlyyellow = 0;
        int onlygsryellow = 0;
        bool isSteeringDetermined = false;

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
                //std::cout << "lambda: groundSteering = " << gsr.groundSteering() << std::endl;
            };

            od4.dataTrigger(opendlv::proxy::GroundSteeringRequest::ID(), onGroundSteeringRequest);

            cv::Mat img(HEIGHT, WIDTH, CV_8UC4);

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
                cv::Mat imageLEFT;
                cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
                // Draw the sample time point on the image
                cv::Mat imgColorSpaceBLUE;
                cv::inRange(imgHSV, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBLUE);
                cv::Mat imgColorSpaceYELLOW;
                cv::inRange(imgHSV, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYELLOW);
                //cv::Mat imgColorSpace = imgColorSpaceBLUE;
                cv::rectangle(img, cv::Point(180, 250), cv::Point(500, 400), cv::Scalar(0,0,255));
                cv::Rect cent(180, 250, 350, 150);
                cv::Mat imageCenter = imgColorSpaceBLUE(cent);
                cv::Mat imageCenter2 = imgColorSpaceYELLOW(cent);
                cv::imshow("Image", img);
                cv::imshow("CENTER Color-Space Image", imageCenter);
                cv::imshow("CENTER Color-Space Image2", imageCenter2);

                if (!isSteeringDetermined) {
                    cv::Rect roi(0, 0, 320, 480);
                    imageLEFT = imgHSV(roi);
                    cv::Mat imgColorSpaceBLUE2;
                    cv::inRange(imageLEFT, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBLUE);
                    hasBlue = cv::countNonZero(imgColorSpaceBLUE2);
                    if (hasBlue > 30) {
                        isBLueLeft = true;
                        std::cout << "BLUE detected!" << std::endl;
                    }
                    isSteeringDetermined = true;
                }

                //cv::rectangle(img, cv::Point(0, 150), cv::Point(200, 700), cv::Scalar(255,0,0));
                hasBlue = 0;
                hasBlue = cv::countNonZero(imageCenter);
                // If you want to access the latest received ground steering, don't forget to lock the mutex:

                {
                    std::lock_guard <std::mutex> lck(gsrMutex);
                    // this is when the car should turn right
                    if (hasBlue > 250) {
                        /*
                        std::cout << "hasBlue=";
                        std::cout << hasBlue;
                         */
                        // TODO: create logic to come up with some ground steering value
                        blue = true;
                    }
                    // this is when the car actually turns right
                    if (isBLueLeft) {
                        if (gsr.groundSteering() < 0) {
                            gsrbool = true;
                        }
                    } else {
                        if (gsr.groundSteering() > 0) {
                            gsrbool = true;
                        }
                    }
                    if (blue && gsrbool){
                        countBlue++;
                    }
                    if (blue && !gsrbool){
                        onlyblue++;
                    }
                    if (!blue && gsrbool){
                        onlygsrblue++;
                    }
                    std::cout << "Correct blue  : ";
                    std::cout << countBlue << std::endl;
                    std::cout << "onlyBlue      : ";
                    std::cout << onlyblue << std::endl;
                    std::cout << "only Gsr right: ";
                    std::cout << onlygsrblue << std::endl;
                    //std::cout << ourGroundSteering;
                    //std::cout << gsr.groundSteering();
                    blue = false;
                    gsrbool = false;

                    hasYellow = 0;
                    hasYellow = cv::countNonZero(imageCenter2);
                    // this is when the car should turn left
                    if (hasYellow > 250) {
                        // TODO: create logic to come up with some ground steering value
                        yellow = true;
                    }
                    if (isBLueLeft) {
                        if (gsr.groundSteering() > 0) {
                            gsrbool = true;
                        }
                    } else {
                        if (gsr.groundSteering() < 0) {
                            gsrbool = true;
                        }
                    }
                    if (yellow && gsrbool){
                        countYellow++;
                    }
                    if (yellow && !gsrbool){
                        onlyyellow++;
                    }
                    if (!yellow && gsrbool){
                        onlygsryellow++;
                    }
                    std::cout << "Correct yellow: ";
                    std::cout << countYellow << std::endl;
                    std::cout << "onlyYellow    : ";
                    std::cout << onlyblue << std::endl;
                    std::cout << "only Gsr left : ";
                    std::cout << onlygsryellow << std::endl;
                    //std::cout << ourGroundSteering;
                    //std::cout << gsr.groundSteering();
                    yellow = false;
                    gsrbool = false;
                }

            }
        }
        retCode = 0;
    }
    return retCode;
}