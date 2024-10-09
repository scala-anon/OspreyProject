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
    std::ifstream inFile("/home/nick/Documents/data/mic1-8-CH17-20240511-121442.dat", std::ios::binary);
    std::ofstream outFile("output.txt");

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

        // Convert binary data to text
        outFile << "Magic: " << packet.magic[0] << packet.magic[1] << "\n";
        outFile << "Message ID: " << packet.msgID << "\n";
        outFile << "Body Length: " << packet.bodyLength << "\n";
        outFile << "Seconds (POSIX Epoch): " << packet.seconds << "\n";
        outFile << "Nanoseconds: " << packet.nanoseconds << "\n";

        // Now, read the body bytes if there are any
        if (packet.bodyLength > 0) {
            // Use std::vector instead of new[]
            std::vector<char> bodyData(packet.bodyLength);
            if (!inFile.read(bodyData.data(), packet.bodyLength)) {
                std::cerr << "Error reading body data." << std::endl;
                return 1;
            }

            // Write body bytes as hex or ASCII
            outFile << "Body Bytes (hex): ";
            for (uint32_t i = 0; i < packet.bodyLength; ++i) {
                outFile << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)bodyData[i] << " ";
            }
            outFile << "\n";
        }
        outFile << "---------------------------\n";  // Separator between packets
    }

    inFile.close();
    outFile.close();

    std::cout << "Binary file converted to text successfully!" << std::endl;
    std::cout << "Total number of packets: " << packetCount << std::endl;
    return 0;
}
