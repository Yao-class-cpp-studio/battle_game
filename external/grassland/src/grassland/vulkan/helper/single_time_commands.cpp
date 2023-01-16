#include <grassland/vulkan/helper/single_time_commands.h>
#include <grassland/vulkan/queue.h>

namespace grassland::vulkan::helper {
void SingleTimeCommands(CommandPool *command_pool,
                        std::function<void(VkCommandBuffer)> actions) {
  std::unique_ptr<CommandBuffer> command_buffer =
      std::make_unique<CommandBuffer>(command_pool);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer->GetHandle(), &beginInfo);

  actions(command_buffer->GetHandle());

  vkEndCommandBuffer(command_buffer->GetHandle());

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffer->GetHandle();

  const auto graphics_queue = command_pool->GetDevice()->GetGraphicsQueue();

  vkQueueSubmit(graphics_queue->GetHandle(), 1, &submitInfo, nullptr);
  graphics_queue->WaitIdle();
}

void SingleTimeCommands(CommandPool *command_pool,
                        std::function<void(CommandBuffer *)> actions) {
  std::unique_ptr<CommandBuffer> command_buffer =
      std::make_unique<CommandBuffer>(command_pool);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer->GetHandle(), &beginInfo);

  actions(command_buffer.get());

  vkEndCommandBuffer(command_buffer->GetHandle());

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffer->GetHandle();

  const auto graphics_queue = command_pool->GetDevice()->GetGraphicsQueue();

  vkQueueSubmit(graphics_queue->GetHandle(), 1, &submitInfo, nullptr);
  graphics_queue->WaitIdle();
}
}  // namespace grassland::vulkan::helper
