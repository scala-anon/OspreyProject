#include <iostream>
#include <fstream>
#include <iomanip>  // For formatting
#include <cstdint>  // For fixed-width integer types like uint32_t, etc.
#include <endian.h> // For byte order conversion functions
#include <vector>   // For std::vector

#pragma pack(push, 1)  // Disable padding to ensure structure matches binary format

struct Packet {
    char magic[2];         // "P" and "S" (2 bytes)
    uint16_t msgID;        // Message ID (2 bytes)
    uint32_t bodyLength;   // Body length (4 bytes)
    uint32_t seconds;      // POSIX timestamp seconds (4 bytes)
    uint32_t nanoseconds;  // Nanoseconds (4 bytes)
    // The body bytes will be read separately as their size is variable
};

#pragma pack(pop)  // Re-enable normal padding

int main() {
    std::ifstream inFile("/home/nicholas/Documents/mic1-8-20240511/mic1-8-CH17-20240511-121442.dat", std::ios::binary);
    std::ofstream outFile("/home/nicholas/Documents/ReformattedFiles/output.txt");

    if (!inFile) {
        std::cerr << "Unable to open the file!" << std::endl;
        return 1;
    }

    Packet packet;
    int packetCount = 0;  // Packet counter

    while (inFile.read(reinterpret_cast<char*>(&packet), sizeof(Packet))) {
        // Check magic bytes
        if (packet.magic[0] != 'P' || packet.magic[1] != 'S') {
            std::cerr << "Invalid magic bytes at packet " << packetCount << std::endl;
            return 1;
        }

        // Convert fields from big-endian to host byte order
        packet.msgID = be16toh(packet.msgID);
        packet.bodyLength = be32toh(packet.bodyLength);
        packet.seconds = be32toh(packet.seconds);
        packet.nanoseconds = be32toh(packet.nanoseconds);

        packetCount++;  // Increment the packet counter

        // Output packet header information
        outFile << "Packet #" << packetCount << "\n";
        outFile << "Magic: " << packet.magic[0] << packet.magic[1] << "\n";
        outFile << "Message ID: " << packet.msgID << "\n";
        outFile << "Body Length: " << packet.bodyLength << "\n";
        outFile << "Seconds: " << packet.seconds << "\n";
        outFile << "Nanoseconds: " << packet.nanoseconds << "\n";

        // Process body bytes if present
        if (packet.bodyLength > 0) {
            std::vector<char> bodyData(packet.bodyLength);
            if (!inFile.read(bodyData.data(), packet.bodyLength)) {
                std::cerr << "Error reading body data." << std::endl;
                return 1;
            }

            // Interpret body as ADC values if msgID is known
            if (packet.msgID == 20033 || packet.msgID == 20034) {
                outFile << "ADC Values: ";

                bool first = true;
                for (size_t i = 0; i + 3 <= packet.bodyLength; i += 3) {
                    int32_t adcValue = ((bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2]);
                    // Sign extend if needed for 24-bit signed integers
                    if (adcValue & 0x800000) {
                        adcValue |= ~0xFFFFFF;
                    }

                    // Output ADC values in comma-separated format
                    if (!first) {
                        outFile << ", ";
                    }
                    outFile << adcValue;
                    first = false;
                }
                outFile << "\n";
            }
        }
        outFile << "---------------------------\n";  // Separator between packets
    }

    inFile.close();
    outFile.close();

    std::cout << "Binary file converted to text successfully!" << std::endl;
    std::cout << "Total number of packets: " << packetCount << std::endl;
    return 0;
}
