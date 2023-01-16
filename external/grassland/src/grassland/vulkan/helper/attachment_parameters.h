#pragma once
#include <grassland/vulkan/util.h>

namespace grassland::vulkan::helper {
class AttachmentParameters {
 public:
  void AddColorAttachment(
      VkFormat format,
      VkSampleCountFlagBits sample_flag = VK_SAMPLE_COUNT_1_BIT);
  void AddDepthStencilAttachment(
      VkFormat format,
      VkSampleCountFlagBits sample_flag = VK_SAMPLE_COUNT_1_BIT);
  [[nodiscard]] const std::vector<VkAttachmentDescription>
      &GetAttachmentDescriptions() const;
  [[nodiscard]] const std::vector<VkAttachmentReference>
      &GetColorAttachmentReferences() const;
  [[nodiscard]] const VkAttachmentReference &GetDepthAttachmentReference()
      const;

 private:
  std::vector<VkAttachmentDescription> attachment_descriptions_{};
  std::vector<VkAttachmentReference> color_attachment_references_{};
  std::unique_ptr<VkAttachmentReference> depth_attachment_reference_{};
};
}  // namespace grassland::vulkan::helper
