#include <grassland/vulkan/helper/attachment_parameters.h>

namespace grassland::vulkan::helper {

void AttachmentParameters::AddColorAttachment(
    VkFormat format,
    VkSampleCountFlagBits sample_flag) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = format;
  colorAttachment.samples = sample_flag;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
  attachment_descriptions_.push_back(colorAttachment);

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = attachment_descriptions_.size() - 1;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment_references_.push_back(colorAttachmentRef);
}

void AttachmentParameters::AddDepthStencilAttachment(
    VkFormat format,
    VkSampleCountFlagBits sample_flag) {
  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = format;
  depthAttachment.samples = sample_flag;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
  attachment_descriptions_.push_back(depthAttachment);

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = attachment_descriptions_.size() - 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depth_attachment_reference_ =
      std::make_unique<VkAttachmentReference>(depthAttachmentRef);
}

const std::vector<VkAttachmentDescription>
    &AttachmentParameters::GetAttachmentDescriptions() const {
  return attachment_descriptions_;
}

const std::vector<VkAttachmentReference>
    &AttachmentParameters::GetColorAttachmentReferences() const {
  return color_attachment_references_;
}

const VkAttachmentReference &AttachmentParameters::GetDepthAttachmentReference()
    const {
  return *depth_attachment_reference_;
}

}  // namespace grassland::vulkan::helper
