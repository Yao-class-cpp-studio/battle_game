#pragma once
#include <grassland/vulkan/vulkan.h>

namespace grassland::vulkan {
uint32_t FormatSizeInBytes(VkFormat format);
uint32_t FormatSlot(VkFormat format);
}  // namespace grassland::vulkan
