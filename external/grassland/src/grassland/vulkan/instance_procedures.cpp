#include "grassland/vulkan/instance_procedures.h"

#include "grassland/util/util.h"

namespace grassland::vulkan {

namespace {
template <class FuncTy>
FuncTy GetProcedure(VkInstance instance, const char *function_name) {
  auto func = (FuncTy)vkGetInstanceProcAddr(instance, function_name);
  return func;
};
}  // namespace

#define GET_PROCEDURE(instance, function_name)                             \
  function_name##_ = grassland::vulkan::GetProcedure<PFN_##function_name>( \
      instance, #function_name)

InstanceProcedures *InstanceProcedures::GetStaticInstance() {
  static InstanceProcedures device_procedures{};
  return &device_procedures;
}

void InstanceProcedures::SetInstance(VkInstance instance) {
  instance_ = instance;
  GET_PROCEDURE(instance_, vkCreateDebugUtilsMessengerEXT);
  GET_PROCEDURE(instance_, vkDestroyDebugUtilsMessengerEXT);
}

VkInstance InstanceProcedures::GetInstance() const {
  return instance_;
}
VkResult InstanceProcedures::vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
  return vkCreateDebugUtilsMessengerEXT_(instance, pCreateInfo, pAllocator,
                                         pMessenger);
}

void InstanceProcedures::vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks *pAllocator) {
  vkDestroyDebugUtilsMessengerEXT_(instance, messenger, pAllocator);
}
}  // namespace grassland::vulkan

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
  return grassland::vulkan::InstanceProcedures::GetStaticInstance()
      ->vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator,
                                       pMessenger);
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT messenger,
                                const VkAllocationCallbacks *pAllocator) {
  grassland::vulkan::InstanceProcedures::GetStaticInstance()
      ->vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
