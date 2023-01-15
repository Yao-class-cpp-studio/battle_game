#pragma once
#include "grassland/vulkan/util.h"

namespace grassland::vulkan::helper {
void CommandBegin(VkCommandBuffer command_buffer);

void CommandEnd(VkCommandBuffer command_buffer);
}  // namespace grassland::vulkan::helper
