#pragma once

#include "grassland/vulkan/image.h"
#include "vulkan/vulkan.h"

namespace grassland::vulkan {
class InstanceProcedures {
 public:
  static InstanceProcedures *GetStaticInstance();
  void SetInstance(VkInstance instance);
  [[nodiscard]] VkInstance GetInstance() const;
  VkResult vkCreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pMessenger);
  void vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT messenger,
                                       const VkAllocationCallbacks *pAllocator);

 private:
  VkInstance instance_{nullptr};
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkCreateDebugUtilsMessengerEXT);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkDestroyDebugUtilsMessengerEXT);
};
}  // namespace grassland::vulkan
