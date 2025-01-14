#ifndef PACKETPARSER_H
#define PACKETPARSER_H

#include <vector>
#include <string>
#include <cstdint>

class PacketParser {
public:
    explicit PacketParser(const std::string& filePath);

    void parseFile();
    const std::vector<int32_t>& getAdcValues() const;

private:
    std::string filePath_;
    std::vector<int32_t> adcValues_;

    void processMessage20033(const std::vector<uint8_t>& bodyData);
    void processMessage20034(const std::vector<uint8_t>& bodyData);
};

#endif // PACKETPARSER_H

