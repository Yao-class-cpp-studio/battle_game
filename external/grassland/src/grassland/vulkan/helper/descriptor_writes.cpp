#include <grassland/vulkan/helper/descriptor_writes.h>

namespace grassland::vulkan::helper {

void UpdateDescriptorWrite(VkDevice &device,
                           VkDescriptorSet &descriptor_set,
                           int binding,
                           Buffer *buffer) {
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = buffer->GetHandle();
  bufferInfo.offset = 0;
  bufferInfo.range = buffer->Size();

  VkWriteDescriptorSet descriptorWrite{};

  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = descriptor_set;
  descriptorWrite.dstBinding = binding;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo = &bufferInfo;

  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void UpdateDescriptorWrite(VkDevice &device,
                           VkDescriptorSet &descriptor_set,
                           int binding,
                           ImageView *image_view,
                           Sampler *sampler) {
  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  imageInfo.imageView = image_view->GetHandle();
  imageInfo.sampler = sampler->GetHandle();

  VkWriteDescriptorSet descriptorWrite{};

  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = descriptor_set;
  descriptorWrite.dstBinding = binding;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pImageInfo = &imageInfo;

  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}
}  // namespace grassland::vulkan::helper
