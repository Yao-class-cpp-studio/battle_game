#pragma once
#include <grassland/vulkan/util.h>

namespace grassland::vulkan::helper {
class DescriptorSetLayoutBindings {
 public:
  void AddBinding(VkDescriptorSetLayoutBinding binding);
  void AddBinding(uint32_t binding,
                  VkDescriptorType descriptorType,
                  uint32_t descriptorCount,
                  VkShaderStageFlags stageFlags,
                  const VkSampler *pImmutableSamplers = nullptr);
  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>
      &GetDescriptorSetLayoutBinding() const;

 private:
  std::vector<VkDescriptorSetLayoutBinding> bindings_;
};
}  // namespace grassland::vulkan::helper
