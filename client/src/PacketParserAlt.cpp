#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <sstream>

// Helper functions for endianness
uint16_t toBigEndian16(const uint8_t* data) {
    return (data[0] << 8) | data[1];
}

uint32_t toBigEndian32(const uint8_t* data) {
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

// Header structure for the binary file
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
            std::cout << "Framing Marker[0]: 0x" << std::hex << static_cast<int>(framingMarker[0]) << std::endl;
            std::cout << "Framing Marker[1]: 0x" << std::hex << static_cast<int>(framingMarker[1]) << std::endl;
            std::cout << std::dec; // Switch back to decimal output if needed
            throw std::runtime_error("Invalid framing marker");
        }
    }
};

// Function to parse and print ADC data
void printADCData(const std::vector<int32_t>& adcData) {
    std::cout << "  ADC Data Count: " << adcData.size() << std::endl;
    std::cout << "  ADC Data: ";
    for (size_t i = 0; i < adcData.size(); ++i) {
        std::cout << adcData[i];
        if (i < adcData.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

// Process Message20033
void processMessage20033(const std::vector<uint8_t>& bodyData) {
    if (bodyData.size() < 0x18) { // Validate minimum size
        throw std::runtime_error("Invalid body size for Message20033");
    }

    // Parse the message
    uint32_t status = toBigEndian32(&bodyData[0]);
    uint32_t activeAdcBitmap = toBigEndian32(&bodyData[4]);
    uint32_t sequenceNumber = toBigEndian32(&bodyData[8]);
    uint32_t seconds = toBigEndian32(&bodyData[0x10]);
    uint32_t nanoseconds = toBigEndian32(&bodyData[0x14]);

    std::vector<int32_t> adcData;
    for (size_t i = 0x18; i + 3 <= bodyData.size(); i += 3) {
        int32_t value = (bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2];
        if (value & 0x800000) {
            value |= ~0xFFFFFF; // Sign extension for 24-bit integers
        }
        adcData.push_back(value);
    }

    // Print parsed data
    std::cout << "Message 20033:" << std::endl;
    std::cout << "  Status: " << status << std::endl;
    std::cout << "  Active ADC Bitmap: " << activeAdcBitmap << std::endl;
    std::cout << "  Sequence Number: " << sequenceNumber << std::endl;
    std::cout << "  Seconds: " << seconds << std::endl;
    std::cout << "  Nanoseconds: " << nanoseconds << std::endl;

    printADCData(adcData);
}

// Process Message20034
void processMessage20034(const std::vector<uint8_t>& bodyData) {
    if (bodyData.size() < 0x28) { // Validate minimum size
        throw std::runtime_error("Invalid body size for Message20034");
    }

    // Parse the message
    uint32_t status = toBigEndian32(&bodyData[0]);
    uint32_t activeAdcBitmap = toBigEndian32(&bodyData[4]);
    uint32_t sequenceNumber = toBigEndian32(&bodyData[8]);
    uint32_t seconds = toBigEndian32(&bodyData[0x10]);
    uint32_t nanoseconds = toBigEndian32(&bodyData[0x14]);
    uint32_t loloChannels = toBigEndian32(&bodyData[0x18]);
    uint32_t loChannels = toBigEndian32(&bodyData[0x1C]);
    uint32_t hiChannels = toBigEndian32(&bodyData[0x20]);
    uint32_t hihiChannels = toBigEndian32(&bodyData[0x24]);

    std::vector<int32_t> adcData;
    for (size_t i = 0x28; i + 3 <= bodyData.size(); i += 3) {
        int32_t value = (bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2];
        if (value & 0x800000) {
            value |= ~0xFFFFFF; // Sign extension for 24-bit integers
        }
        adcData.push_back(value);
    }

    // Print parsed data
    std::cout << "Message 20034:" << std::endl;
    std::cout << "  Status: " << status << std::endl;
    std::cout << "  Active ADC Bitmap: " << activeAdcBitmap << std::endl;
    std::cout << "  Sequence Number: " << sequenceNumber << std::endl;
    std::cout << "  Seconds: " << seconds << std::endl;
    std::cout << "  Nanoseconds: " << nanoseconds << std::endl;
    std::cout << "  LOLO Channels: " << loloChannels << std::endl;
    std::cout << "  LO Channels: " << loChannels << std::endl;
    std::cout << "  HI Channels: " << hiChannels << std::endl;
    std::cout << "  HIHI Channels: " << hihiChannels << std::endl;

    printADCData(adcData);
}

// Main function
int main() {
    const std::string filePath = "/home/nicholas/OspreyData/mic1-8-20240511/mic1-8-CH17-20240511-121442.dat";

    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return 1;
    }

    int i = 500;
    while (i <= 500 ) {
        uint8_t headerBytes[16]; // Update to 16 bytes to include 8-byte reception timestamp
        std::streampos startPos = inFile.tellg();

        if (!inFile.read(reinterpret_cast<char*>(headerBytes), sizeof(headerBytes))) {
            if (inFile.eof()) {
                std::cout << "End of file reached." << std::endl;
                break;
            } else {
                std::cerr << "Failed to read header!" << std::endl;
                return 1;
            }
        }

        try {
            Header header(headerBytes);
            std::cout << "Parsing started at offset: " << startPos << std::endl;
            std::cout << "Parsing completed at offset: " << inFile.tellg() << std::endl;

            std::vector<uint8_t> bodyData(header.bodyLength);
            if (!inFile.read(reinterpret_cast<char*>(bodyData.data()), bodyData.size())) {
                std::cerr << "Failed to read body data!" << std::endl;
                break;
            }

            if (header.messageID == 20033) {
                processMessage20033(bodyData);
            } else if (header.messageID == 20034) {
                processMessage20034(bodyData);
            } else {
                std::cerr << "Unknown message ID: " << header.messageID << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Error occurred at offset: " << inFile.tellg() << std::endl;
        }
        i++;
    }

    return 0;
}
