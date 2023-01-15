#include <grassland/util/logging.h>
#include <grassland/vulkan/render_pass.h>

namespace grassland::vulkan {

RenderPass::RenderPass(Device *device, VkFormat color_format) {
  helper::AttachmentParameters attachment_parameters;
  attachment_parameters.AddColorAttachment(color_format);
  ConstructorCommon(device, attachment_parameters);
}

RenderPass::RenderPass(Device *device,
                       VkFormat color_format,
                       VkFormat depth_format) {
  helper::AttachmentParameters attachment_parameters;
  attachment_parameters.AddColorAttachment(color_format);
  attachment_parameters.AddDepthStencilAttachment(depth_format);
  ConstructorCommon(device, attachment_parameters);
}

RenderPass::RenderPass(
    Device *device,
    const helper::AttachmentParameters &attachment_parameters) {
  ConstructorCommon(device, attachment_parameters);
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(device_->GetHandle(), handle_, nullptr);
}

void RenderPass::ConstructorCommon(
    Device *device,
    const helper::AttachmentParameters &attachment_parameters) {
  handle_ = {};
  device_ = device;

  auto &attachment_references =
      attachment_parameters.GetColorAttachmentReferences();
  auto &attachment_descriptions =
      attachment_parameters.GetAttachmentDescriptions();

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = attachment_references.size();
  subpass.pColorAttachments = attachment_references.data();
  subpass.pDepthStencilAttachment =
      &attachment_parameters.GetDepthAttachmentReference();
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = attachment_descriptions.size();
  renderPassInfo.pAttachments = attachment_descriptions.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device->GetHandle(), &renderPassInfo, nullptr,
                         &handle_) != VK_SUCCESS) {
    LAND_ERROR("failed to create render pass!");
  }
}
}  // namespace grassland::vulkan
