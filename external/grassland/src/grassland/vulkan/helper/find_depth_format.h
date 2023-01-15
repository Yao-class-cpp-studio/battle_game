#pragma once
#include <grassland/vulkan/physical_device.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan::helper {

VkFormat FindSupportedFormat(PhysicalDevice *physical_device,
                             const std::vector<VkFormat> &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);

VkFormat FindDepthFormat(PhysicalDevice *physical_device);
}  // namespace grassland::vulkan::helper
