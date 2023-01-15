#include <grassland/util/logging.h>
#include <grassland/vulkan/pipeline_layout.h>

namespace grassland::vulkan {
PipelineLayout::PipelineLayout(Device *device)
    : PipelineLayout(device, std::vector<VkDescriptorSetLayout>{}) {
}

PipelineLayout::PipelineLayout(Device *device,
                               DescriptorSetLayout *descriptor_set_layout)
    : PipelineLayout(device, {descriptor_set_layout->GetHandle()}) {
}

PipelineLayout::PipelineLayout(
    Device *device,
    const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts)
    : handle_{} {
  device_ = device;
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = descriptor_set_layouts.size();
  if (pipelineLayoutInfo.setLayoutCount) {
    pipelineLayoutInfo.pSetLayouts = descriptor_set_layouts.data();
  }
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutInfo, nullptr,
                             &handle_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create pipeline layout!");
  }
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(device_->GetHandle(), handle_, nullptr);
}
}  // namespace grassland::vulkan
