#include <grassland/util/logging.h>
#include <grassland/vulkan/command_buffer.h>

namespace grassland::vulkan {

CommandBuffer::CommandBuffer(CommandPool *command_pool) : handle_{} {
  command_pool_ = command_pool;
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool_->GetHandle();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1u;

  if (vkAllocateCommandBuffers(command_pool_->GetDevice()->GetHandle(),
                               &allocInfo, &handle_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to allocate command buffer!");
  }
}

CommandBuffer::~CommandBuffer() {
  vkFreeCommandBuffers(command_pool_->GetDevice()->GetHandle(),
                       command_pool_->GetHandle(), 1, &handle_);
}

CommandPool *CommandBuffer::GetCommandPool() const {
  return command_pool_;
}

CommandBuffers::CommandBuffers(CommandPool *command_pool, uint32_t count) {
  command_pool_ = command_pool;
  count_ = count;
  command_buffers_.resize(count);
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool_->GetHandle();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = count;

  if (vkAllocateCommandBuffers(command_pool_->GetDevice()->GetHandle(),
                               &allocInfo,
                               command_buffers_.data()) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to allocate command buffers!");
  }
}

CommandBuffers::~CommandBuffers() {
  vkFreeCommandBuffers(command_pool_->GetDevice()->GetHandle(),
                       command_pool_->GetHandle(), count_,
                       command_buffers_.data());
}

VkCommandBuffer CommandBuffers::GetHandle(int buffer_index) {
  return command_buffers_[buffer_index];
}

VkCommandBuffer &CommandBuffers::operator[](int buffer_index) {
  return command_buffers_[buffer_index];
}

const VkCommandBuffer &CommandBuffers::operator[](int buffer_index) const {
  return command_buffers_[buffer_index];
}

CommandPool *CommandBuffers::GetCommandPool() const {
  return command_pool_;
}

}  // namespace grassland::vulkan
