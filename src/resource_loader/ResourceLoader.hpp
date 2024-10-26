//
// Created by kido on 10/26/24.
//

#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H
#include "loader/VulkanImageLoader.hpp"
#include <iostream>

namespace vz {

class ResourceLoader {
public:
    static bool loadVulkanImage(const std::string& path) {
        return m_imageLoader.load(path);
    }
    static VulkanImage* getVulkanImage(const std::string& path) {
        return m_imageLoader.get(path);
    }
private:
    static VulkanImageLoader m_imageLoader;
    ResourceLoader() = default;
    ~ResourceLoader() = default;
};

}
#endif
