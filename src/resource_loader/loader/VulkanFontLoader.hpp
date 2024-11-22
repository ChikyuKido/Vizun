#ifndef VULKANFONTLOADER_HPP
#define VULKANFONTLOADER_HPP
#include "Loader.hpp"

namespace vz {
class VulkanFont;
class VulkanFontLoader : public Loader<VulkanFont>{
public:
    bool load(const std::string& path,float size);
    VulkanFont* get(const std::string& path,float size);
private:
    bool load(const std::string& path) override;;
};

}

#endif
