#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "utils/stats.hpp"
#include "data/steering.hpp"
#include "utils/fileio.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <atomic>
#include <signal.h>

std::atomic<bool> should_exit{false};
static bool isConeColorDetermined{false};

// Signal handler function
void signalHandler(int signum)
{
    // explicitly ignore signum
    (void) signum;
    should_exit = true;
}

int main(int argc, char **argv)
{
    int retCode{1};

    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);

    if ((0 == commandlineArguments.count("name")) ||
        (0 == commandlineArguments.count("width")) ||
        (0 == commandlineArguments.count("height")) ||
        (0 == commandlineArguments.count("cid")))
    {

        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image and transform it to HSV color space for inspection." << std::endl;
        std::cerr << "Usage: " << argv[0] << " --name=<name of shared memory area> --width=<W> --height=<H> --cid=<C>" << std::endl;
        std::cerr << " --name: name of the shared memory area to attach" << std::endl;
        std::cerr << " --width: width of the frame" << std::endl;
        std::cerr << " --height: height of the frame" << std::endl;
        std::cerr << " --cid: given cid" << std::endl;
        std::cerr << "Example: " << argv[0] << " --name=img.argb --width=640 --height=480 --cid=253" << std::endl;
    }
    else
    {
        // extract commandline arguments
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
        const uint16_t CID{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

        // signal handler for ctrl+c
        signal(SIGINT, signalHandler);

        // create shared memory pointer
        std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};

        // while shared memory is not valid, keep attempting to recreate it
        while (!sharedMemory->valid() && !should_exit)
        {
            sharedMemory = std::make_unique<cluon::SharedMemory>(NAME);
            std::cout << "Waiting for shared memory region to be created..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (sharedMemory && sharedMemory->valid())
        {
            std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

            // Interface to a running OpenDaVINCI session where network messages are exchanged.
            // The instance od4 allows you to send and receive messages.
            cluon::OD4Session od4{CID};

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

            opendlv::proxy::VoltageReading leftVoltage;  // Declare an instance of opendlv::proxy::VoltageReading called "leftVoltage".
            opendlv::proxy::VoltageReading rightVoltage; // Declare an instance of opendlv::proxy::VoltageReading called "rightVoltage".

            std::mutex voltageMutex; // Declare a mutex object called "voltageMutex" to protect the shared voltage readings.

            auto VoltageReading = [&](cluon::data::Envelope &&envelope) // Declare a lambda function called "VoltageReading" that takes a cluon::data::Envelope by rvalue reference.
            {
                std::lock_guard<std::mutex> lck(voltageMutex); // Lock the "voltageMutex" so that only one thread can access the shared voltage readings at a time.

                leftVoltage = cluon::extractMessage<opendlv::proxy::VoltageReading>(std::move(envelope));  // Extract the left voltage reading from the cluon::data::Envelope and store it in "leftVoltage".
                rightVoltage = cluon::extractMessage<opendlv::proxy::VoltageReading>(std::move(envelope)); // Extract the right voltage reading from the cluon::data::Envelope and store it in "rightVoltage".
            };

            od4.dataTrigger(opendlv::proxy::VoltageReading::ID(), VoltageReading); // Trigger the "VoltageReading" lambda function when a message with the ID of opendlv::proxy::VoltageReading is received.

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
                int64_t sampleTimeStamp = cluon::time::toMicroseconds(sharedMemory->getTimeStamp().second);
                sharedMemory->unlock();
                std::stringstream final;
                final << sampleTimeStamp;
                //std::cout << "group_06;" << final.str() << ";" << steeringWheelAngle << std::endl;

                // define the color spaces used for blue and yellow cones
                cv::Mat imgHSV;
                cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
                cv::Mat imgColorSpaceBlue;
                cv::inRange(imgHSV, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBlue);
                cv::Mat imgColorSpaceYellow;
                cv::inRange(imgHSV, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYellow);
                cv::rectangle(img, cv::Point(180, 250), cv::Point(500, 400), cv::Scalar(0, 0, 255));
                
                // define the center areas of focus for the cv algorithm 
                cv::Rect centerLeft(160, 250, 330, 150);
                cv::Rect centerRight(180, 250, 370, 150);
                cv::Mat imgCenterLeft = imgColorSpaceBlue(centerLeft);
                cv::Mat imgCenterRight = imgColorSpaceYellow(centerRight);

                // determine the side of the colored cones, performed until a color is decided upon
                isConeColorDetermined = !isConeColorDetermined ? determineConeColors(imgColorSpaceBlue, imgColorSpaceYellow, centerLeft, centerRight) : isConeColorDetermined;

                // If you want to access the latest received ground steering, don't forget to lock the mutex:
                {
                    std::lock_guard<std::mutex> lck(gsrMutex);
                    // calculateStats(imgCenterLeft, imgCenterRight, gsr, isBlueLeft);
                    float g1 = gsr.groundSteering();
                    float g2 = getGSR(imgCenterLeft, imgCenterRight, leftVoltage, rightVoltage);
                    determineError(g1, g2);
                    //writePixels(cv::countNonZero(imgCenterLeft), cv::countNonZero(imgCenterRight), gsr.groundSteering(), g2);
                     std::string filename = "/host/data.csv";

                     std::stringstream file_data;
                    // Append formatted data to the string stream "data"
                    file_data << g1 << "," << final.str();

                    // Write the previous and current commit values to the CSV file
                    write_file(filename, std::to_string(g2), file_data.str());

                }  
            }
        }
        retCode = 0;
    }
    return retCode;
}