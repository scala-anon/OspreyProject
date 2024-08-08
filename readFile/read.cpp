#include <iostream>
#include <fstream>
#include <string>

// Function to read and print the contents of a .hdr file line by line
void readAndPrintHDRFile(const std::string& filename) {
    std::ifstream inputFile(filename);

    if (!inputFile) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::cout << line << std::endl;
    }

    inputFile.close();
}

int main() {
    std::string filename = "/home/nick/Documents/data/mic1-8.hdr"; // Replace with your actual .hdr file name
    readAndPrintHDRFile(filename);

    return 0;
}
