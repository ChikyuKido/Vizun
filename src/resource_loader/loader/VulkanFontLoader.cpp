#include "VulkanFontLoader.hpp"
#include "graphics/resources/VulkanFont.hpp"
#include "utils/Logger.hpp"

namespace vz {
bool VulkanFontLoader::load(const std::string& path, float size) {
    VulkanFont* font = new VulkanFont();
    font->loadFont(path,size);
    m_data[path+std::to_string(size)] = font;
    return true;
}

VulkanFont* VulkanFontLoader::get(const std::string& path, float size) {
    return m_data[path+std::to_string(size)];
}

bool VulkanFontLoader::load(const std::string& path) {
    VZ_LOG_CRITICAL("NOT IMPLEMENTED");
}
}