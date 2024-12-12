#ifndef PACKETPARSER_H
#define PACKETPARSER_H

#include <vector>
#include <string>

class PacketParser {
public:
    explicit PacketParser(const std::string& filePath);
    void parseFile();
    const std::vector<int32_t>& getAdcValues() const;

private:
    std::string filePath_;
    std::vector<int32_t> adcValues_;
};

#endif // PACKETPARSER_H

