#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <ctime>

#pragma pack(push, 1)

// Body structure for MSGID 1
struct BodyType1 {
    uint32_t channelData[32];  // 32 channels of 32-bit unsigned integers
};

// Body structure for MSGID 2
struct BodyType2 {
    float signalStrength[32];  // 32 channels of floats for signal strength
};

#pragma pack(pop)

// PSC Header structure with updated format
#pragma pack(push, 1)
struct PSCHeader {
    char magic[2];          // 'P', 'S' (2 bytes)
    uint16_t msgID;         // Message ID (2 bytes)
    uint32_t bodyLength;    // Body length (4 bytes)
    uint32_t seconds;       // POSIX timestamp (4 bytes)
    uint32_t nanoseconds;   // Nanoseconds (4 bytes)
};
#pragma pack(pop)

// Body structure for dynamic processing
void interpretBody(const PSCHeader& header, const std::vector<char>& bodyData) {
    if (header.msgID == 1) {
        // Interpret the body as BodyType1
        BodyType1 body;
        std::memcpy(&body, bodyData.data(), sizeof(BodyType1));
        
        // Process the data from BodyType1
        std::cout << "Processing BodyType1 data (MSGID 1):" << std::endl;
        for (int i = 0; i < 32; ++i) {
            std::cout << "Channel " << i << ": " << body.channelData[i] << std::endl;
        }
    }
    else if (header.msgID == 2) {
        // Interpret the body as BodyType2
        BodyType2 body;
        std::memcpy(&body, bodyData.data(), sizeof(BodyType2));

        // Process the data from BodyType2
        std::cout << "Processing BodyType2 data (MSGID 2):" << std::endl;
        for (int i = 0; i < 32; ++i) {
            std::cout << "Signal Strength " << i << ": " << body.signalStrength[i] << std::endl;
        }
    }
    else {
        // Unknown MSGID
        std::cerr << "Unknown MSGID: " << header.msgID << std::endl;
    }
}

void readPSCFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << fileName << std::endl;
        return;
    }

    // Loop through the file, reading each header and body
    while (file) {
        PSCHeader header;

        // Read the header
        file.read(reinterpret_cast<char*>(&header), sizeof(PSCHeader));

        // Check if the file still has data to process
        if (!file) {
            break;  // End of file or error reading header
        }

        // Ensure the header starts with 'P' and 'S'
        if (header.magic[0] != 'P' || header.magic[1] != 'S') {
            std::cerr << "Invalid PSC Header Magic!" << std::endl;
            break;
        }

        // Convert the POSIX timestamp to a readable format
        std::time_t posix_time = static_cast<std::time_t>(header.seconds);
        std::tm* timeInfo = std::gmtime(&posix_time);
        char timeString[32];
        std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeInfo);

        // Output timestamp information
        std::cout << "Message received at: " << timeString << "." << header.nanoseconds << " UTC" << std::endl;

        // Read the body data based on the body length in the header
        std::vector<char> bodyData(header.bodyLength);
        file.read(bodyData.data(), header.bodyLength);

        if (!file) {
            std::cerr << "Error: Could not read body data!" << std::endl;
            break;
        }

        // Dynamically process the body based on the MSGID
        interpretBody(header, bodyData);
    }

    file.close();
}

int main() {
    // Test the file reading and processing
    readPSCFile("/home/niko/Documents/OspreyProject-1/mic1-8-20240511/mic1-8-CH17-20240511-121442.dat");
    return 0;
}
