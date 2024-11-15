
#include "FileUtil.hpp"

#include "Logger.hpp"
#include <fstream>

namespace vz {
std::vector<uint8_t> FileUtil::readFileAsBytes(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        VZ_LOG_CRITICAL("Failed to open file %s", filePath.c_str());
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        VZ_LOG_CRITICAL("Failed to read file %s", filePath.c_str());
    }

    return buffer;
}
}