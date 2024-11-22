#ifndef FILEUTIL_HPP
#define FILEUTIL_HPP
#include <string>
#include <vector>

namespace vz {
class FileUtil {
public:
    static std::vector<uint8_t> readFileAsBytes(const std::string& filePath);
};
}

#endif
