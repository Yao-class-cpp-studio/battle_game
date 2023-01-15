#pragma once
#include <grassland/vulkan/device.h>

namespace grassland::vulkan {
class Semaphore {
 public:
  explicit Semaphore(Device *device);
  ~Semaphore();

 private:
  GRASSLAND_VULKAN_HANDLE(VkSemaphore)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
