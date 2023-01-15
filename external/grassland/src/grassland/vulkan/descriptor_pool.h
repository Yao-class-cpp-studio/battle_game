#pragma once
#include <grassland/vulkan/device.h>
#include <grassland/vulkan/helper/descriptor_set_layout_bindings.h>

namespace grassland::vulkan {
class DescriptorPool {
 public:
  DescriptorPool(Device *device,
                 const helper::DescriptorSetLayoutBindings &bindings,
                 int max_sets);
  DescriptorPool(Device *device,
                 const std::vector<VkDescriptorSetLayoutBinding> &bindings,
                 int max_sets);
  ~DescriptorPool();

 private:
  GRASSLAND_VULKAN_HANDLE(VkDescriptorPool)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
