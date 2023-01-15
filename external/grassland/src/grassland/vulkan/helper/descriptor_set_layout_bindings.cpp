#include <grassland/vulkan/helper/descriptor_set_layout_bindings.h>

namespace grassland::vulkan::helper {

void DescriptorSetLayoutBindings::AddBinding(
    VkDescriptorSetLayoutBinding binding) {
  bindings_.push_back(binding);
}

void DescriptorSetLayoutBindings::AddBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    uint32_t descriptorCount,
    VkShaderStageFlags stageFlags,
    const VkSampler *pImmutableSamplers) {
  VkDescriptorSetLayoutBinding layout_binding{};
  layout_binding.binding = binding;
  layout_binding.descriptorType = descriptorType;
  layout_binding.descriptorCount = descriptorCount;
  layout_binding.stageFlags = stageFlags;
  layout_binding.pImmutableSamplers = pImmutableSamplers;
  bindings_.push_back(layout_binding);
}

const std::vector<VkDescriptorSetLayoutBinding>
    &DescriptorSetLayoutBindings::GetDescriptorSetLayoutBinding() const {
  return bindings_;
}
}  // namespace grassland::vulkan::helper
