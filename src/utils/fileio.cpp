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

void write_file(const std::string& filename, const std::string& current_commit, const std::string& data) {
    static bool removeFile = true;
    static bool header_exists = false;
    // Check if the file already exists
    bool fileExists = std::filesystem::exists(filename);

    // Open the file in append mode or create a new file in append mode
    std::ofstream file;
    if (fileExists && removeFile) {
        // Remove the file if it exists
        std::filesystem::remove(filename);
    }

    if (!header_exists) {
        write_header_row(filename);
        header_exists = true;
    }

    file.open(filename, std::ios_base::app);
    removeFile = false;

    // Check if the file was opened successfully
    if (!file.is_open()) {
        // Output an error message to the standard error stream and return
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }

    // Write the current commit value to the file
    file << current_commit << ",";

    // Write the additional data to the file
    file << data << std::endl;

    // Flush and close the file stream
    file.close();
}
