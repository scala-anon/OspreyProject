#include "FileReader.h"
#include <fstream>
#include <iostream>

std::vector<Sample> ReadSignalFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return {};
    }

    std::vector<Sample> samples;

    for (int i = 0; i < 1000; ++i) {
        uint8_t type;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));

        Sample sample;

        switch (type) {
            case 1: {  // int
                int value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                sample.value = value;
                break;
            }
            case 2: {  // float
                float value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                sample.value = value;
                break;
            }
            case 3: {  // string
                uint16_t length;
                file.read(reinterpret_cast<char*>(&length), sizeof(length));
                std::string value(length, '\0');
                file.read(value.data(), length);
                sample.value = value;
                break;
            }
            // Add more case handlers as needed for other types
        }

        samples.push_back(sample);
    }

    return samples;
}
