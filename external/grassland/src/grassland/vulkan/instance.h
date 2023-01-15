#pragma once
#include <grassland/vulkan/util.h>
#include <vulkan/vulkan.h>

namespace grassland::vulkan {
class Instance {
 public:
  explicit Instance(
      bool require_surface = true,
      bool enable_validation_layers = kDefaultEnableValidationLayers);
  ~Instance();

 private:
  void CreateInstance();
  void CreateDebugMessenger();
  GRASSLAND_VULKAN_HANDLE(VkInstance)
  VkDebugUtilsMessengerEXT debug_messenger_{};
  bool require_surface_{true};
  bool enable_validation_layers_{true};
};
}  // namespace grassland::vulkan
