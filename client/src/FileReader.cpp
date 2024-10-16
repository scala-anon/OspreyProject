#include <iostream>
#include <fstream>
#include <iomanip>  // For formatting
#include <cstdint>  // For fixed-width integer types like uint32_t, etc.
#include <endian.h> // For byte order conversion functions
#include <vector>   // For std::vector
#include <bitset>   // For binary representation of flags

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

// Function to decode the Status field
void decodeStatus(uint32_t status, std::ostream &out) {
    out << "Status:\n";
    out << "  Calibration factors invalid: " << ((status & (1 << 4)) ? "Yes" : "No") << "\n";
    out << "  Packet transmission overrun: " << ((status & (1 << 3)) ? "Yes" : "No") << "\n";
    out << "  Packet building overrun: " << ((status & (1 << 2)) ? "Yes" : "No") << "\n";
    out << "  Time of day may be invalid: " << ((status & (1 << 1)) ? "Yes" : "No") << "\n";
    out << "  System clock PLL unlocked: " << ((status & 1) ? "Yes" : "No") << "\n";
}

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

            // Interpret Status, Active ADC bitmap, and Sequence #
            uint32_t status = (bodyData[0x00] << 24) | (bodyData[0x01] << 16) | (bodyData[0x02] << 8) | bodyData[0x03];
            uint32_t activeAdcBitmap = (bodyData[0x04] << 24) | (bodyData[0x05] << 16) | (bodyData[0x06] << 8) | bodyData[0x07];
            uint64_t sequenceNumber = 
                (static_cast<uint64_t>(bodyData[0x08]) << 56) | (static_cast<uint64_t>(bodyData[0x09]) << 48) |
                (static_cast<uint64_t>(bodyData[0x0A]) << 40) | (static_cast<uint64_t>(bodyData[0x0B]) << 32) |
                (static_cast<uint64_t>(bodyData[0x0C]) << 24) | (static_cast<uint64_t>(bodyData[0x0D]) << 16) |
                (static_cast<uint64_t>(bodyData[0x0E]) << 8)  | static_cast<uint64_t>(bodyData[0x0F]);

            // Decode Status and output Active ADC Bitmap and Sequence #
            decodeStatus(status, outFile);
            outFile << "Active ADC Bitmap: " << std::bitset<32>(activeAdcBitmap) << "\n";
            outFile << "Sequence Number: " << sequenceNumber << "\n";

            // Interpret body based on Message ID
            if (packet.msgID == 20033) { // Message type NA
                outFile << "ADC Values (comma-separated): ";
                bool first = true;
                for (size_t i = 0x18; i + 3 <= packet.bodyLength; i += 3) {  // ADC values start at offset 0x18
                    int32_t adcValue = ((bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2]);
                    if (adcValue & 0x800000) {
                        adcValue |= ~0xFFFFFF;
                    }
                    if (!first) {
                        outFile << ", ";
                    }
                    outFile << adcValue;
                    first = false;
                }
                outFile << "\n";

            } else if (packet.msgID == 20034) { // Message type NB
                // Interpret LOLO, LO, HI, and HIHI channels as bitmaps
                uint32_t loloChannels = (bodyData[0x18] << 24) | (bodyData[0x19] << 16) | (bodyData[0x1A] << 8) | bodyData[0x1B];
                uint32_t loChannels = (bodyData[0x1C] << 24) | (bodyData[0x1D] << 16) | (bodyData[0x1E] << 8) | bodyData[0x1F];
                uint32_t hiChannels = (bodyData[0x20] << 24) | (bodyData[0x21] << 16) | (bodyData[0x22] << 8) | bodyData[0x23];
                uint32_t hihiChannels = (bodyData[0x24] << 24) | (bodyData[0x25] << 16) | (bodyData[0x26] << 8) | bodyData[0x27];

                outFile << "LOLO Channels: " << std::bitset<32>(loloChannels) << "\n";
                outFile << "LO Channels: " << std::bitset<32>(loChannels) << "\n";
                outFile << "HI Channels: " << std::bitset<32>(hiChannels) << "\n";
                outFile << "HIHI Channels: " << std::bitset<32>(hihiChannels) << "\n";
                outFile << "ADC Values (comma-separated): ";

                bool first = true;
                for (size_t i = 0x28; i + 3 <= packet.bodyLength; i += 3) {  // ADC values start at offset 0x28
                    int32_t adcValue = ((bodyData[i] << 16) | (bodyData[i + 1] << 8) | bodyData[i + 2]);
                    if (adcValue & 0x800000) {
                        adcValue |= ~0xFFFFFF;
                    }
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
