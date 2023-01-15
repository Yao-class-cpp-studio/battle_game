#pragma once
#include <grassland/vulkan/device.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan {
class ShaderModule {
 public:
  explicit ShaderModule(Device *device, const char *spv_file_path);
  explicit ShaderModule(Device *device, const std::vector<uint8_t> &spv_data);
  ~ShaderModule();

 private:
  GRASSLAND_VULKAN_HANDLE(VkShaderModule)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
