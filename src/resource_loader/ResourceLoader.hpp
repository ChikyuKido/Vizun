#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H
#include "loader/VulkanFontLoader.hpp"
#include "loader/VulkanImageLoader.hpp"

namespace vz {

class ResourceLoader {
public:
    static bool loadVulkanImage(const std::string& path) {
        return m_imageLoader.load(path);
    }
    static VulkanImage* getVulkanImage(const std::string& path) {
        return m_imageLoader.get(path);
    }
    static bool loadVulkanFont(const std::string& path,float size) {
        return m_fontLoader.load(path,size);
    }
    static VulkanFont* getVulkanFont(const std::string& path,float size) {
        return m_fontLoader.get(path,size);
    }
private:
    static VulkanImageLoader m_imageLoader;
    static VulkanFontLoader m_fontLoader;
    ResourceLoader() = default;
    ~ResourceLoader() = default;
};

}
#endif
