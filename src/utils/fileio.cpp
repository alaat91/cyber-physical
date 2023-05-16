#include <fstream>
#include <sstream>
#include "fileio.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>

void write_header_row(const std::string& filename) {
    // Open the file in write mode
    std::ofstream file(filename);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        // Output an error message to the standard error stream and return
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }

    // Write the header row to the file
    file << "current_commit," << "GroundSteeringRequest," << "sampleTimeStamp" << std::endl;

    // Flush and close the file stream
    file.flush();
    file.close();
}

void write_file(const std::string& current_commit, const std::string& data) {
    static bool createFile = true;
    static bool header_exists = false;
    int counter = 1;

    std::ofstream file;
    static std::string newFilename = "/host/data.csv";

    if(createFile){
        while (std::filesystem::exists(newFilename)) {
            newFilename = "/host/data-" + std::to_string(counter++) +".csv";
        }
        createFile = false;
    }
    
    if (!header_exists) {
        write_header_row(newFilename);
        header_exists = true;
    }

    file.open(newFilename, std::ios_base::app); 

    if (!file.is_open()) {
        // Output an error message to the standard error stream and return
        std::cerr << "Error opening file " << newFilename << std::endl;
        return;
    }

    // Write the current commit value to the file
    file << current_commit << ",";

    // Write the additional data to the file
    file << data << std::endl;

    // Flush and close the file stream
    file.close();
}
