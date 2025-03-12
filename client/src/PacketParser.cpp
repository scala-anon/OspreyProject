#include "PacketParser.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

// Convert a 16-bit value from big-endian to host order
uint16_t toBigEndian16(const uint8_t* data) {
    return (data[0] << 8) | data[1]; // Shift the first byte to the upper 8 bits and OR with the second byte
}

// Convert a 32-bit value from big-endian to host order
uint32_t toBigEndian32(const uint8_t* data) {
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]; // Construct the 32-bit integer by shifting and ORing
}

// Header structure for parsing packets
struct Header {
    char framingMarker[2];   // 2 bytes for ASCII framing marker ('P' and 'S')
    uint16_t messageID;      // 2-byte message ID (big-endian format)
    uint32_t bodyLength;     // 4-byte body length (big-endian format)

    // Constructor to parse header data
    explicit Header(const uint8_t* data) {
        framingMarker[0] = data[0]; // Assign the first framing character
        framingMarker[1] = data[1]; // Assign the second framing character
        messageID = toBigEndian16(&data[2]); // Convert 2-byte message ID to host order
        bodyLength = toBigEndian32(&data[4]); // Convert 4-byte body length to host order

        // Validate that the packet starts with the correct framing marker ('P' and 'S')
        if (framingMarker[0] != 'P' || framingMarker[1] != 'S') {
            throw std::runtime_error("Invalid framing marker"); // Throw an error if incorrect
        }
    }
};

// Constructor for the PacketParser class, initializing the file path
PacketParser::PacketParser(const std::string& filePath) : filePath_(filePath) {}

void PacketParser::parseFile() {
    // Open the binary file for reading
    std::ifstream inFile(filePath_, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file: " + filePath_); // Error if file cannot be opened
    }

    // Loop through the file, reading each packet
    while (true) {
        uint8_t headerBytes[16]; // Buffer for storing the header (includes reception timestamp)

        // Attempt to read the header
        if (!inFile.read(reinterpret_cast<char*>(headerBytes), sizeof(headerBytes))) {
            if (inFile.eof()) {
                break; // End of file reached, stop parsing
            } else {
                throw std::runtime_error("Failed to read header"); // Error if reading fails
            }
        }

        // Parse the header data
        Header header(headerBytes);

        // Read the message body based on the parsed body length
        std::vector<uint8_t> bodyData(header.bodyLength);
        if (!inFile.read(reinterpret_cast<char*>(bodyData.data()), bodyData.size())) {
            throw std::runtime_error("Failed to read body data"); // Error if body reading fails
        }

        // Process the message based on its ID
        if (header.messageID == 20033) {
            processMessage20033(bodyData);
        } else if (header.messageID == 20034) {
            processMessage20034(bodyData);
        }
    }
}

// Returns the ADC values extracted from parsed messages
const std::vector<int32_t>& PacketParser::getAdcValues() const {
    return adcValues_;
}

// Process message type 20033 (ADC Data Format 1)
void PacketParser::processMessage20033(const std::vector<uint8_t>& bodyData) {
    // Iterate through the body, starting at offset 0x18, processing 3-byte ADC values
    for (size_t i = 0x18; i + 3 <= bodyData.size(); i += 3) {
        int32_t value = (bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2]; // Construct the 24-bit value

        // Check if the value is negative (24-bit sign extension)
        if (value & 0x800000) {
            value |= ~0xFFFFFF; // Extend sign for negative numbers
        }

        adcValues_.push_back(value); // Store the extracted ADC value
    }
}

// Process message type 20034 (ADC Data Format 2)
void PacketParser::processMessage20034(const std::vector<uint8_t>& bodyData) {
    // Iterate through the body, starting at offset 0x28, processing 3-byte ADC values
    for (size_t i = 0x28; i + 3 <= bodyData.size(); i += 3) {
        int32_t value = (bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2]; // Construct the 24-bit value

        // Check if the value is negative (24-bit sign extension)
        if (value & 0x800000) {
            value |= ~0xFFFFFF; // Extend sign for negative numbers
        }

        adcValues_.push_back(value); // Store the extracted ADC value
    }
}

