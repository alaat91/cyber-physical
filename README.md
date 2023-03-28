
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

3. Create a build directory and navigate to it:
```
mkdir build && cd build
```

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

## Authors

* **Armin Balesic** - [balesic](https://git.chalmers.se/balesic)
* **Alaa Taleb** - [alaat](https://git.chalmers.se/alaat)
* **Umar Mahmood** - [umarma](https://git.chalmers.se/umarma)
* **Shariq Shahbaz** - [shariqs](https://git.chalmers.se/shariqs)
* **Victor Campanello** - [viccam](https://git.chalmers.se/viccam)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

