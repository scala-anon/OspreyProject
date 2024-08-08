#include <iostream>
#include <fstream>
#include <iomanip>

void printLineAsHex(const std::string& line) {
    for (unsigned char c : line) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;
}

void printLineAsASCII(const std::string& line) {
    for (unsigned char c : line) {
        if (std::isprint(c)) {
            std::cout << c;
        } else {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

int main() {
    std::ifstream inputFile("data/mic1-8-CH17-20240511-121442.dat", std::ios::binary);

    if (!inputFile) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        // Output as hex
        std::cout << "Hex: ";
        printLineAsHex(line);

        // Output as ASCII
        std::cout << "ASCII: ";
        printLineAsASCII(line);

        std::cout << std::endl; // Separator between lines
    }

    inputFile.close();
    return 0;
}
