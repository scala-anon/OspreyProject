#include "PacketParser.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

uint16_t toBigEndian16(const uint8_t* data) {
    return (data[0] << 8) | data[1];
}

uint32_t toBigEndian32(const uint8_t* data) {
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

// Header structure for parsing
struct Header {
    char framingMarker[2];   // 2 bytes for ASCII 'P' and 'S'
    uint16_t messageID;      // 2 bytes for message ID (big-endian)
    uint32_t bodyLength;     // 4 bytes for body length (big-endian)

    explicit Header(const uint8_t* data) {
        framingMarker[0] = data[0];
        framingMarker[1] = data[1];
        messageID = toBigEndian16(&data[2]);
        bodyLength = toBigEndian32(&data[4]);

        if (framingMarker[0] != 'P' || framingMarker[1] != 'S') {
            throw std::runtime_error("Invalid framing marker");
        }
    }
};

PacketParser::PacketParser(const std::string& filePath) : filePath_(filePath) {}

void PacketParser::parseFile() {
    std::ifstream inFile(filePath_, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file: " + filePath_);
    }

    while (true) {
        uint8_t headerBytes[16]; // Include reception timestamp
        if (!inFile.read(reinterpret_cast<char*>(headerBytes), sizeof(headerBytes))) {
            if (inFile.eof()) {
                break;
            } else {
                throw std::runtime_error("Failed to read header");
            }
        }

        Header header(headerBytes);

        std::vector<uint8_t> bodyData(header.bodyLength);
        if (!inFile.read(reinterpret_cast<char*>(bodyData.data()), bodyData.size())) {
            throw std::runtime_error("Failed to read body data");
        }

        if (header.messageID == 20033) {
            processMessage20033(bodyData);
        } else if (header.messageID == 20034) {
            processMessage20034(bodyData);
        }
    }
}

const std::vector<int32_t>& PacketParser::getAdcValues() const {
    return adcValues_;
}

void PacketParser::processMessage20033(const std::vector<uint8_t>& bodyData) {
    for (size_t i = 0x18; i + 3 <= bodyData.size(); i += 3) {
        int32_t value = (bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2];
        if (value & 0x800000) {
            value |= ~0xFFFFFF; // Sign extension for 24-bit integers
        }
        adcValues_.push_back(value);
    }
}

void PacketParser::processMessage20034(const std::vector<uint8_t>& bodyData) {
    for (size_t i = 0x28; i + 3 <= bodyData.size(); i += 3) {
        int32_t value = (bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2];
        if (value & 0x800000) {
            value |= ~0xFFFFFF; // Sign extension for 24-bit integers
        }
        adcValues_.push_back(value);
    }
}

