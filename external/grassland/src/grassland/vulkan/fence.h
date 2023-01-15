#pragma once
#include <grassland/vulkan/device.h>

namespace grassland::vulkan {
class Fence {
 public:
  explicit Fence(Device *device);
  ~Fence();

 private:
  GRASSLAND_VULKAN_HANDLE(VkFence)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
