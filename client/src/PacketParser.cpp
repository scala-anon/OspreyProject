#include "PacketParser.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdint>

PacketParser::PacketParser(const std::string& filePath) : filePath_(filePath) {}

void PacketParser::parseFile() {
    std::ifstream inFile(filePath_, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file: " + filePath_);
    }

    // Example: Parse 24-bit ADC values from the binary file
    while (!inFile.eof()) {
        uint8_t buffer[3];
        if (!inFile.read(reinterpret_cast<char*>(buffer), 3)) {
            break;
        }

        int32_t value = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
        if (value & 0x800000) {
            value |= ~0xFFFFFF;
        }
        adcValues_.push_back(value);
    }

    inFile.close();
}

const std::vector<int32_t>& PacketParser::getAdcValues() const {
    return adcValues_;
}

