#pragma once

#include <grassland/vulkan/device.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan {
class CommandPool {
 public:
  explicit CommandPool(Device *device);
  ~CommandPool();

 private:
  GRASSLAND_VULKAN_HANDLE(VkCommandPool)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
