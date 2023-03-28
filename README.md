# Project Title

## Getting Started

These instructions will help you get a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

To build this project, you will need to have CMake and Make installed on your system. The installation may vary depending on your operating system, but the instructions for a Debian based system (e.g Ubuntu) are listed below. You can install them using the following commands:

```
sudo apt-get install cmake sudo apt-get install build-essential
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

3. Create a build directory and navigate to it:
```
mkdir build && cd build
```

4. Run CMake to generate the Makefiles:
```
cmake ..
```

5. Run Make to build the project:
```
make
```

After completing these steps, you should have an executable file in the `build` directory that you can run to test the project.

## Running the Tests

To run the tests for this project, navigate to the `build` directory and run the `make test` command.

## Built With

* [CMake](https://cmake.org/) - Build system
* [Make](https://www.gnu.org/software/make/) - Build automation tool

## Authors

* **Armin Balesic** - [balesic](https://git.chalmers.se/balesic)
* **Alaa Taleb** - [alaat](https://git.chalmers.se/alaat)
* **Umar Mahmood** - [umarma](https://git.chalmers.se/umarma)
* **Shariq Shahbaz** - [shariqs](https://git.chalmers.se/shariqs)
* **Victor Campanello** - [viccam](https://git.chalmers.se/viccam)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

