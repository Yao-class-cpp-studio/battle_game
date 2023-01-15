#pragma once
#include <grassland/vulkan/physical_device.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan {

class Queue;

class Device {
 public:
  explicit Device(
      PhysicalDevice *physical_device,
      const std::vector<const char *> &extra_device_extensions = {},
      bool enable_validation_layers = kDefaultEnableValidationLayers);
  Device(PhysicalDevice *physical_device,
         Surface *surface,
         const std::vector<const char *> &extra_device_extensions = {},
         bool enable_validation_layers = kDefaultEnableValidationLayers,
         void *extraDeviceFeatures = nullptr);
  ~Device();
  [[nodiscard]] PhysicalDevice *GetPhysicalDevice() const;
  [[nodiscard]] Surface *GetSurface() const;
  [[nodiscard]] Queue *GetGraphicsQueue() const;
  [[nodiscard]] Queue *GetPresentQueue() const;
  void WaitIdle();

 private:
  GRASSLAND_VULKAN_HANDLE(VkDevice)
  PhysicalDevice *physical_device_{nullptr};
  Surface *surface_{nullptr};
  std::unique_ptr<Queue> graphics_queue_;
  std::unique_ptr<Queue> present_queue_;
};
}  // namespace grassland::vulkan
