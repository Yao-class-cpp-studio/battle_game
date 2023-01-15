#include <grassland/util/logging.h>
#include <grassland/vulkan/descriptor_set.h>

namespace grassland::vulkan {

DescriptorSet::DescriptorSet(Device *device,
                             DescriptorSetLayout *descriptor_set_layout,
                             DescriptorPool *descriptor_pool)
    : handle_{} {
  device_ = device;
  descriptor_pool_ = descriptor_pool;
  VkDescriptorSetLayout layout = descriptor_set_layout->GetHandle();
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptor_pool->GetHandle();
  allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
  allocInfo.pSetLayouts = &layout;

  if (vkAllocateDescriptorSets(device_->GetHandle(), &allocInfo, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to allocate descriptor set!");
  }
}

DescriptorSet::DescriptorSet(DescriptorSetLayout *descriptor_set_layout,
                             DescriptorPool *descriptor_pool)
    : DescriptorSet(descriptor_pool->GetDevice(),
                    descriptor_set_layout,
                    descriptor_pool) {
}

DescriptorSet::~DescriptorSet() = default;

DescriptorSets::DescriptorSets(Device *device,
                               DescriptorSetLayout *descriptor_set_layout,
                               DescriptorPool *descriptor_pool,
                               uint32_t set_count) {
  device_ = device;
  descriptor_pool_ = descriptor_pool;
  descriptor_sets_.resize(set_count);
  std::vector<VkDescriptorSetLayout> layouts(
      set_count, descriptor_set_layout->GetHandle());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptor_pool->GetHandle();
  allocInfo.descriptorSetCount = set_count;
  allocInfo.pSetLayouts = layouts.data();

  if (vkAllocateDescriptorSets(device_->GetHandle(), &allocInfo,
                               descriptor_sets_.data()) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to allocate descriptor sets!");
  }
}

DescriptorSets::~DescriptorSets() {
  vkFreeDescriptorSets(device_->GetHandle(), descriptor_pool_->GetHandle(),
                       descriptor_sets_.size(), descriptor_sets_.data());
}

VkDescriptorSet &DescriptorSets::GetHandle(uint32_t index) {
  return descriptor_sets_[index];
}

const VkDescriptorSet &DescriptorSets::GetHandle(uint32_t index) const {
  return descriptor_sets_[index];
}

}  // namespace grassland::vulkan
