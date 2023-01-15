#pragma once
#include <grassland/vulkan/command_pool.h>

namespace grassland::vulkan {
class CommandBuffer {
 public:
  explicit CommandBuffer(CommandPool *command_pool);
  ~CommandBuffer();
  [[nodiscard]] CommandPool *GetCommandPool() const;

 private:
  GRASSLAND_VULKAN_HANDLE(VkCommandBuffer)
  CommandPool *command_pool_{nullptr};
};

class CommandBuffers {
 public:
  CommandBuffers(CommandPool *command_pool, uint32_t count);
  ~CommandBuffers();
  VkCommandBuffer GetHandle(int buffer_index);
  VkCommandBuffer &operator[](int buffer_index);
  const VkCommandBuffer &operator[](int buffer_index) const;
  [[nodiscard]] CommandPool *GetCommandPool() const;

 private:
  std::vector<VkCommandBuffer> command_buffers_{};
  CommandPool *command_pool_{nullptr};
  uint32_t count_{0};
};
}  // namespace grassland::vulkan
