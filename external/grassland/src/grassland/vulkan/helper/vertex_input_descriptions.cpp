#include <grassland/vulkan/helper/vertex_input_descriptions.h>

namespace grassland::vulkan::helper {

void VertexInputDescriptions::AddBinding(uint32_t binding, uint32_t stride) {
  VkVertexInputBindingDescription description{};
  description.binding = binding;
  description.stride = stride;
  description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  binding_descriptions_.push_back(description);
}

void VertexInputDescriptions::AddAttribute(uint32_t binding,
                                           uint32_t location,
                                           VkFormat format,
                                           uint32_t offset) {
  VkVertexInputAttributeDescription description;
  description.location = location;
  description.binding = binding;
  description.format = format;
  description.offset = offset;
  attribute_descriptions_.push_back(description);
}
const std::vector<VkVertexInputBindingDescription>
    &VertexInputDescriptions::GetBindingDescriptions() const {
  return binding_descriptions_;
}

const std::vector<VkVertexInputAttributeDescription>
    &VertexInputDescriptions::GetAttributeDescription() const {
  return attribute_descriptions_;
}
}  // namespace grassland::vulkan::helper
