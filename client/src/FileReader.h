#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <vector>
#include <variant>

// Define the type for a sample
using SampleType = std::variant<int, long, float, double, std::string, bool, std::vector<uint8_t>>;

struct Sample {
    SampleType value;  // Holds the actual value of the sample
};

// Function to read signal files and return a vector of samples
std::vector<Sample> ReadSignalFile(const std::string& filePath);

#endif // FILEREADER_H
