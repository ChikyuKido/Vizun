#ifndef VULKANFONTLOADER_HPP
#define VULKANFONTLOADER_HPP
#include "Loader.hpp"
#include "graphics/resources/VulkanFont.hpp"
#include "utils/Logger.hpp"

namespace vz {
class VulkanFontLoader : public Loader<VulkanFont>{
public:
    bool load(const std::string& path,float size) {
        VulkanFont* font = new VulkanFont();
        font->loadFont(path,size);
        m_data[path+std::to_string(size)] = font;
        return true;
    }
    VulkanFont* get(const std::string& path,float size) {
        return m_data[path+std::to_string(size)];
    }
private:
    bool load(const std::string& path) override {
        VZ_LOG_CRITICAL("NOT IMPLEMENTED");
    };
};
}

#endif
