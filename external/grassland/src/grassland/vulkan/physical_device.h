#pragma once
#include <grassland/vulkan/instance.h>
#include <grassland/vulkan/surface.h>
#include <grassland/vulkan/util.h>

#include <functional>
#include <string>
#include <vector>

namespace grassland::vulkan {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice {
 public:
  PhysicalDevice();
  explicit PhysicalDevice(VkPhysicalDevice handle);
  [[nodiscard]] std::string DeviceName() const;
  [[nodiscard]] uint32_t GraphicsFamilyIndex() const;
  uint32_t PresentFamilyIndex(Surface *surface) const;
  [[nodiscard]] bool HasPresentationSupport() const;
  [[nodiscard]] bool IsDiscreteGPU() const;
  [[nodiscard]] bool HasGeometryShader() const;
  [[nodiscard]] bool HasRayTracingPipeline() const;
  [[nodiscard]] uint64_t DeviceMemorySize() const;
  bool SwapChainAdequate(Surface *surface) const;
  void PrintDeviceProperties() const;
  void PrintDeviceFeatures() const;
  SwapChainSupportDetails GetSwapChainSupportDetails(Surface *surface) const;
  [[nodiscard]] std::vector<VkExtensionProperties> GetExtensions() const;

  [[nodiscard]] VkPhysicalDeviceFeatures GetFeatures() const;
  [[nodiscard]] VkPhysicalDeviceProperties GetProperties() const;
  [[nodiscard]] VkPhysicalDeviceRayTracingPipelinePropertiesKHR
  GetRayTracingProperties() const;

 private:
  GRASSLAND_VULKAN_HANDLE(VkPhysicalDevice)
  VkPhysicalDeviceProperties2 properties_{};
  VkPhysicalDeviceFeatures2 features_{};
  VkPhysicalDeviceMemoryProperties memory_properties_{};
  VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_properties_{};
  VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_features_{};
};

std::vector<PhysicalDevice> GetPhysicalDevices(Instance *instance);
PhysicalDevice PickPhysicalDevice(
    const std::vector<PhysicalDevice> &device_list,
    const std::function<int(PhysicalDevice physical_device)> &rate_function);
PhysicalDevice PickPhysicalDevice(
    Instance *instance,
    const std::function<int(PhysicalDevice)> &rate_function);

}  // namespace grassland::vulkan
