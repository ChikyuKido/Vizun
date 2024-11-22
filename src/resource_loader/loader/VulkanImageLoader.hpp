#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include "Loader.hpp"
#include "graphics/resources/VulkanImage.hpp"

namespace vz {
class VulkanImageLoader : public Loader<VulkanImage>{
public:
    ~VulkanImageLoader() override;
    bool load(const std::string& path) override;
};
}

#endif
