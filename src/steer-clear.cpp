#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "utils/stats.hpp"
#include <iostream>
#include <iomanip> // include the header file for setprecision
#include <fstream> // include the header file for file stream
#include <string>
#include <cmath>

float correctFrames = 0;
float totalFrames = 0;

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

        // Create an output file stream object named csvFile
        std::ofstream csvFile;

        // Open the file "data.csv" with the "append" flag
        csvFile.open("/host/data.csv", std::ios_base::app);

        // Check if the file was opened successfully
        if (!csvFile.is_open()) {
            // Output an error message to the standard error stream and return an error code of 1
            std::cerr << "Error opening file " << "data.csv" << std::endl;
            return 1;
        }

        // Initialize a float variable named steeringWheelAngle to the value of pi
        float steeringWheelAngle = 3.14159265359f;

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

            opendlv::proxy::VoltageReading leftVoltage;  // Declare an instance of opendlv::proxy::VoltageReading called "leftVoltage".
            opendlv::proxy::VoltageReading rightVoltage; // Declare an instance of opendlv::proxy::VoltageReading called "rightVoltage".

            std::mutex voltageMutex; // Declare a mutex object called "voltageMutex" to protect the shared voltage readings.

            auto VoltageReading = [&](cluon::data::Envelope &&envelope) // Declare a lambda function called "VoltageReading" that takes a cluon::data::Envelope by rvalue reference.
            {
                std::lock_guard<std::mutex> lck(voltageMutex); // Lock the "voltageMutex" so that only one thread can access the shared voltage readings at a time.

                leftVoltage = cluon::extractMessage<opendlv::proxy::VoltageReading>(std::move(envelope)); // Extract the left voltage reading from the cluon::data::Envelope and store it in "leftVoltage".
                rightVoltage = cluon::extractMessage<opendlv::proxy::VoltageReading>(std::move(envelope)); // Extract the right voltage reading from the cluon::data::Envelope and store it in "rightVoltage".

                if (envelope.senderStamp() == 1) { // If the sender ID of the envelope is 1 (left IR sensor):
                    //std::cout << "Left Sensor Voltage: " << std::fixed << std::setprecision(10) << leftVoltage.voltage() << std::endl; // Print the left voltage reading to the console.
                }

                if (envelope.senderStamp() == 3) { // If the sender ID of the envelope is 3 (right IR sensor):
                    //std::cout << "Right Sensor Voltage: " << std::fixed << std::setprecision(10) << rightVoltage.voltage() << std::endl; // Print the right voltage reading to the console.
                }

                /*
                 * Implement basic calculation of steeringWheelAngle.
                 * If leftVoltage is 0.01 or higher, set steeringWheelAngle to -0.04.
                 * If rightVoltage is 0.01 or higher, set steeringWheelAngle to 0.04.
                 * Else, set steeringWheelAngle to zero.
                 */
                if (leftVoltage.voltage() >= 0.089f) {
                    steeringWheelAngle = -0.03f;
                } else if (rightVoltage.voltage() >= 0.089f) {
                    steeringWheelAngle = 0.03f;
                } else {
                    steeringWheelAngle = 0.00f;
                }

                // print value of steeringWheelAngle to console.
                //std::cout << "steeringWheelAngle: " << std::fixed << std::setprecision(6) << steeringWheelAngle << std::endl;
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

                cv::Mat imgHSV;
                cv::Mat imageLEFT;
                cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
                cv::Mat imgColorSpaceBLUE;
                cv::inRange(imgHSV, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBLUE);
                cv::Mat imgColorSpaceYELLOW;
                cv::inRange(imgHSV, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYELLOW);
                cv::rectangle(img, cv::Point(180, 250), cv::Point(500, 400), cv::Scalar(0, 0, 255));

                cv::Rect cent(180, 250, 350, 150);
                cv::Mat imgCenterBlue = imgColorSpaceBLUE(cent);
                cv::Mat imgCenterYellow = imgColorSpaceYELLOW(cent);
                //cv::imshow("Image", img);
                cv::imshow("Center-Frame Color-Space Blue", imgCenterBlue);
                cv::imshow("Center-Frame Color-Space Yellow", imgCenterYellow);

                if (!isSteeringDetermined)
                {
                    cv::Rect roi(0, 0, 320, 480);
                    imageLEFT = imgHSV(roi);
                    cv::inRange(imageLEFT, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBLUE);
                    //cv::imshow("LEFT", imgColorSpaceBLUE);
                    int bluePixels = cv::countNonZero(imgColorSpaceBLUE);
                    if (bluePixels > 30)
                    {
                        isBlueLeft = true;
                        std::cout << "BLUE detected!" << std::endl;
                    }
                    isSteeringDetermined = true;
                }

                // If you want to access the latest received ground steering, don't forget to lock the mutex:
                {
                    // Lock the mutex to prevent other threads from accessing shared resources
                    std::lock_guard<std::mutex> lck(gsrMutex);

                    // Calculate some statistics based on the image data
                    calculateStats(imgCenterBlue, imgCenterYellow, gsr, isBlueLeft);

                    // Output the ground steering request and the current steering wheel angle
                    //std::cout << "GroundSteeringRequest: " << gsr.groundSteering() << std::endl;
                    //std::cout << "steeringWheelAngle: " << std::fixed << std::setprecision(6) << steeringWheelAngle << std::endl;

                    // Calculate the error between the ground steering and the actual steering
                    float groundSteering = gsr.groundSteering();
                    float error = ((fabs(groundSteering - steeringWheelAngle)) / fabs(groundSteering)) * 100;

                    // If the ground steering is zero, set the error to the absolute difference between the two steering angles
                    if(groundSteering == 0){
                        error = fabs(groundSteering-steeringWheelAngle);
                    }

                    // Output the original and ground steering angles, as well as the error
                    //std::cout << "Original Steering: " << groundSteering << std::endl;
                    //std::cout << "Ground Steering: " << steeringWheelAngle << std::endl;
                    //std::cout << "Error: " << error << std::endl;

                    // If the ground steering is not zero and the error is less than or equal to 30, increment the correct frames counter
                    // Otherwise, if the ground steering is zero and the absolute difference between the two steering angles is less than 0.05, increment the correct frames counter
                    // In all other cases, do not increment the correct frames counter
                    if (groundSteering != 0 && error <= 30 ){
                        correctFrames++;
                    } else if(groundSteering==0 && fabs(groundSteering-steeringWheelAngle) < 0.05) {
                        correctFrames++;
                    }

                    // write value of steeringWheelAngle to csvFile.
                    csvFile << "GroundSteeringRequest -> " << groundSteering << "; " << "sampleTimeStamp -> " << final.str() << "; " << "steeringWheelAngle -> " << steeringWheelAngle << std::endl;

                    // print A20 requirement
                    std::cout << "group_06;" << final.str() << ";" << steeringWheelAngle << std::endl;


                    // Increment the total frames counter
                    totalFrames++;
                }

                // Output the frame statistics (i.e., the number of correct frames and the total number of frames processed)
                // as well as the percentage of correct frames
                //std::cout << "Frame Stats:" << std::setprecision(0) << correctFrames << "/" << std::setprecision(0) << totalFrames << "\t" << ((correctFrames/totalFrames)*100) << " %" << std::endl;

            }
        }

        // flush and close csvFile
        csvFile.flush();
        csvFile.close();

        retCode = 0;
    }
    return retCode;
}