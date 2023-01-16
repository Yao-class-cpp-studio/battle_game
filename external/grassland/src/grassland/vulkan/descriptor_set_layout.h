#pragma once
#include <grassland/vulkan/device.h>
#include <grassland/vulkan/helper/descriptor_set_layout_bindings.h>

namespace grassland::vulkan {
class DescriptorSetLayout {
 public:
  DescriptorSetLayout(
      Device *device,
      const helper::DescriptorSetLayoutBindings &layout_bindings);
  DescriptorSetLayout(
      Device *device,
      const std::vector<VkDescriptorSetLayoutBinding> &layout_bindings);
  ~DescriptorSetLayout();

 private:
  GRASSLAND_VULKAN_HANDLE(VkDescriptorSetLayout)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
