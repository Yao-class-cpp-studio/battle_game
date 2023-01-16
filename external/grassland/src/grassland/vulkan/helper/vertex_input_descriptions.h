#pragma once
#include <grassland/vulkan/util.h>

namespace grassland::vulkan::helper {
class VertexInputDescriptions {
 public:
  void AddBinding(uint32_t binding, uint32_t stride);
  void AddAttribute(uint32_t binding,
                    uint32_t location,
                    VkFormat format,
                    uint32_t offset);
  [[nodiscard]] const std::vector<VkVertexInputBindingDescription>
      &GetBindingDescriptions() const;
  [[nodiscard]] const std::vector<VkVertexInputAttributeDescription>
      &GetAttributeDescription() const;

 private:
  std::vector<VkVertexInputBindingDescription> binding_descriptions_{};
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions_{};
};
}  // namespace grassland::vulkan::helper
