#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#define GRASSLAND_VULKAN_HANDLE(type) \
  type handle_{};                     \
                                      \
 public:                              \
  type &GetHandle() {                 \
    return handle_;                   \
  }                                   \
  const type &GetHandle() const {     \
    return handle_;                   \
  }                                   \
                                      \
 private:

#define GRASSLAND_VULKAN_DEVICE_PTR \
  Device *device_{nullptr};         \
                                    \
 public:                            \
  Device *GetDevice() const {       \
    return device_;                 \
  }                                 \
                                    \
 private:

#define GRASSLAND_VULKAN_PROCEDURE_VAR(function_name) \
  PFN_##function_name function_name##_;
namespace grassland::vulkan {
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool kDefaultEnableValidationLayers = false;
#else
constexpr bool kDefaultEnableValidationLayers = true;
#endif
}  // namespace grassland::vulkan
