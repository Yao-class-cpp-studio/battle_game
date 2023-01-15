#pragma once
#include <grassland/vulkan/buffer.h>
#include <grassland/vulkan/descriptor_set.h>
#include <grassland/vulkan/image.h>
#include <grassland/vulkan/image_view.h>
#include <grassland/vulkan/sampler.h>

namespace grassland::vulkan::helper {
void UpdateDescriptorWrite(VkDevice &device,
                           VkDescriptorSet &descriptor_set,
                           int binding,
                           Buffer *buffer);
void UpdateDescriptorWrite(VkDevice &device,
                           VkDescriptorSet &descriptor_set,
                           int binding,
                           ImageView *image_view,
                           Sampler *sampler);
}  // namespace grassland::vulkan::helper
