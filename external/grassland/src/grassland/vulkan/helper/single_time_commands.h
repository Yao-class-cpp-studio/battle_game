#pragma once
#include <grassland/vulkan/command_buffer.h>
#include <grassland/vulkan/command_pool.h>

namespace grassland::vulkan::helper {
void SingleTimeCommands(
    CommandPool *command_pool,
    std::function<void(VkCommandBuffer command_buffer)> actions);
void SingleTimeCommands(
    CommandPool *command_pool,
    std::function<void(CommandBuffer *command_buffer)> actions);
}  // namespace grassland::vulkan::helper
