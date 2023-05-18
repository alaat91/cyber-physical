[![Latest Release](https://git.chalmers.se/courses/dit638/students/2023-group-06/-/badges/release.svg)](https://git.chalmers.se/courses/dit638/students/2023-group-06/-/releases)
[![pipeline status](https://git.chalmers.se/courses/dit638/students/2023-group-06/badges/main/pipeline.svg)](https://git.chalmers.se/courses/dit638/students/2023-group-06/-/pipelines)
[![coverage report](https://git.chalmers.se/courses/dit638/students/2023-group-06/badges/a8-bonus/coverage.svg)](https://git.chalmers.se/courses/dit638/students/2023-group-06/-/graphs/main/charts)

# 2023-group-06

## Getting Started

These instructions will help you get a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

To build this project, you will need to have CMake, Make and G++ installed on your system. The installation may vary depending on your operating system, but the instructions for a Debian based system (e.g Ubuntu) are listed below. You can install them using the following commands:

```
sudo apt-get install cmake build-essential
```

### Building the Project

To build the project, follow these steps:

1. Clone the repository into a clean folder:


```
git clone git@git.chalmers.se:courses/dit638/students/2023-group-06.git
```

2. Navigate to the project's root directory:
```
cd 2023-group-06
```

3. Download Docker and Docker compose if you don't have them:
```
Installation guide for docker: https://docs.docker.com/engine/install/
Intstallion guide for docker compose: https://docs.docker.com/compose/install/


4. Run CMake to generate the Makefiles:
```
cmake ../src
```

5. Run Make to build the project:
```
make
```

After completing these steps, you should have an executable file in the `build` directory that you can run to test the project.

### Running the Tests

To run the tests for this project, navigate to the `build` directory and run the `make test` command.

# Functional Requirements
- The system must receive and process car data from infrared sensors.
- The system must receive and process image data for steering calculations.
- The system should integrate the infrared sensor and image data to make accurate steering decisions.
- The system should adapt the steering angle based on the proximity and position of detected objects.
- The system must provide real-time feedback on the calculated steering angle.
- The system should maintain reliability even in the event of individual hardware crashes.
- The system should handle edge cases within stated limits.
- The application must support a command line parameter --verbose.
- When --verbose is provided, the application must display a debug window marking relevant algorithm features.
- When --verbose is omitted, the application must not display any window and should not crash.
- The application must be able to integrate into the data processing chain using opendlv-vehicle-view to replay data and h264decoder to extract frames.
- The application must receive image information via the shared memory area and should not read video files or separate image files.
- The application must print output on the console per frame in the following format: group_XY;sampleTimeStamp in microseconds;steeringWheelAngle.
- The output must include the group number, sample time stamp, and computed steering wheel angle.

# Non-Functional Requirements
- The system must be faster than 100 milliseconds per frame.
- The steering angle must be within +/- 30% of the original steering angle in more than or equal to 55% of all video frames.
- The steering angle must be +/- 0.05 when the original steering angle is 0.
- The application must not crash when there is no graphical user interface available.
- The application should be able to run on a miniature car without a screen.
- The Docker image must support multiple platforms, including linux/amd64 and linux/arm/v7.
- The application must execute without crashing on both Intel/AMD x86_64 and ARM platforms.

## Working as a Team
### Add new features
For a feature that has already been added to the issue board. 
 * Create a new feature branch 
 * Make the relevant changes 
 * Push code to remote feature branch
 * Create a merge request when feature has been tested and fulfills the acceptance criteria
 * Tag relevant members of the team
 * Fulfill the definition of done for the merge request
 * Assign a reviewer and await response
 * As a reviewer, it is your responsibility to make sure that the DOD is met.
 * Make relevant changes if needed, otherwise merge.
 
 ### Breaking Changes
 * Register a new issue as a bug on the KanBan board
 * Follow the same steps as for adding new features
 
 For new features that have not yet been added to the issue board. A new issue should be created that defines an acceptance criteria as well as label the issue accordingly.
 
   
### The seven rules of a great Git commit message
Separate subject from body with a blank line https://cbea.ms/git-commit/#separate

Limit the subject line to 50 characters https://cbea.ms/git-commit/#limit-50

Capitalize the subject line https://cbea.ms/git-commit/#capitalize

Do not end the subject line with a period https://cbea.ms/git-commit/#end

Use the imperative mood in the subject line https://cbea.ms/git-commit/#imperative

Wrap the body at 72 characters https://cbea.ms/git-commit/#wrap-72

Use the body to explain what and why vs. how https://cbea.ms/git-commit/#why-not-how

## Built With
* [G++](https://gcc.gnu.org/) - Compiler
* [CMake](https://cmake.org/) - Build system
* [Make](https://www.gnu.org/software/make/) - Build automation tool
* [OpenCV](https://opencv.org/) - Computer Vision

## Authors

* **Armin Balesic** - [balesic](https://git.chalmers.se/balesic)
* **Alaa Taleb** - [alaat](https://git.chalmers.se/alaat)
* **Umar Mahmood** - [umarma](https://git.chalmers.se/umarma)
* **Shariq Shahbaz** - [shariqs](https://git.chalmers.se/shariqs)
* **Victor Campanello** - [viccam](https://git.chalmers.se/viccam)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

