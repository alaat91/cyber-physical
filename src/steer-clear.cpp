#include "cluon-complete.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>

bool isSteeringDetermined = false;

int main(int argc, char** argv) {
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
    } else {
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};

        std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};

        if (sharedMemory && sharedMemory->valid()) {
            std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

            cv::Mat img(HEIGHT, WIDTH, CV_8UC4);

            while (cv::waitKey(10)) {
                bool isBlueLeft = false;

                sharedMemory->lock();
                {
                    std::memcpy(img.data, sharedMemory->data(), sharedMemory->size());
                }
                sharedMemory->unlock();

                cv::Mat imgHSV;
                cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);

                if (!isSteeringDetermined) {
                    cv::Rect roi(0, 0, 320, 480);
                    cv::Mat imageLEFT = imgHSV(roi);
                    cv::Mat imgColorSpaceBLUE2;
                    cv::inRange(imageLEFT, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBLUE2);
                    int hasBlue = cv::countNonZero(imgColorSpaceBLUE2);
                    if (hasBlue > 30) {
                        isBlueLeft = true;
                        std::cout << "BLUE detected!" << std::endl;
                    }
                    isSteeringDetermined = true;
                }

                cv::Mat imgColorSpaceBLUE;
                cv::inRange(imgHSV, cv::Scalar(101, 110, 37), cv::Scalar(142, 255, 255), imgColorSpaceBLUE);
                cv::Mat imgColorSpaceYELLOW;
                cv::inRange(imgHSV, cv::Scalar(13, 58, 133), cv::Scalar(26, 255, 255), imgColorSpaceYELLOW);
                cv::Mat imgColorSpace = imgColorSpaceBLUE | imgColorSpaceYELLOW;
                cv::imshow("Color-Space Image", imgColorSpace);
                cv::imshow(sharedMemory->name().c_str(), img);
            }
        }
        retCode = 0;
    }
    return retCode;
}
