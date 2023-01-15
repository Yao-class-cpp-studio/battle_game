#include <grassland/util/logging.h>
#include <grassland/vulkan/helper/commands.h>

namespace grassland::vulkan::helper {

void CommandBegin(VkCommandBuffer command_buffer) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to begin recording command buffer!");
  }
}

void CommandEnd(VkCommandBuffer command_buffer) {
  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to record command buffer!");
  }
}

}  // namespace grassland::vulkan::helper
