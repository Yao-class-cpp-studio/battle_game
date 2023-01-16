#pragma once
#include <grassland/vulkan/descriptor_pool.h>
#include <grassland/vulkan/descriptor_set_layout.h>

namespace grassland::vulkan {
class DescriptorSet {
 public:
  DescriptorSet(Device *device,
                DescriptorSetLayout *descriptor_set_layout,
                DescriptorPool *descriptor_pool);
  DescriptorSet(DescriptorSetLayout *descriptor_set_layout,
                DescriptorPool *descriptor_pool);
  ~DescriptorSet();

 private:
  GRASSLAND_VULKAN_HANDLE(VkDescriptorSet)
  GRASSLAND_VULKAN_DEVICE_PTR
  DescriptorPool *descriptor_pool_{nullptr};
};

class DescriptorSets {
 public:
  DescriptorSets(Device *device,
                 DescriptorSetLayout *descriptor_set_layout,
                 DescriptorPool *descriptor_pool,
                 uint32_t set_count);
  ~DescriptorSets();
  VkDescriptorSet &GetHandle(uint32_t index);
  [[nodiscard]] const VkDescriptorSet &GetHandle(uint32_t index) const;

 private:
  std::vector<VkDescriptorSet> descriptor_sets_;
  GRASSLAND_VULKAN_DEVICE_PTR
  DescriptorPool *descriptor_pool_{nullptr};
};
}  // namespace grassland::vulkan
