#include <fstream>
#include <sstream>
#include "fileio.hpp"
#include <iostream>

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
    file << "current_commit;" << "previous_commit;" << "GroundSteeringRequest;" << "sampleTimeStamp;" << std::endl;

    // Flush and close the file stream
    file.flush();
    file.close();
}

int read_file(const std::string& filename) {
    // Declare a variable to store the previous commit value
    int previous_commit = 0;

    // Read the previous commit value from the CSV file if it exists
    std::ifstream file(filename);
    if (file.good()) {
        std::string line;
        std::getline(file, line);
        std::stringstream linestream(line);
        std::string previous_commit_str;
        std::getline(linestream, previous_commit_str, ';');
        try {
            previous_commit = std::stoi(previous_commit_str);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid previous commit value: " << e.what() << std::endl;
        }
    }
    file.close();

    return previous_commit;
}

// Function to write data to the CSV file
void write_file(const std::string& filename, const std::string& previous_commit, const std::string& current_commit, const std::string& data) {
    // Open the file in append mode
    std::ofstream file(filename, std::ios_base::app);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        // Output an error message to the standard error stream and return
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }

    // Write the previous and current commit values to the file
    file << previous_commit << ";" << current_commit << ";";

    // Write the additional data to the file
    file << data << std::endl;

    // Flush and close the file stream
    file.flush();
    file.close();
}
