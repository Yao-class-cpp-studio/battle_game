#pragma once
#include <grassland/vulkan/device.h>
#include <grassland/vulkan/helper/attachment_parameters.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan {
class RenderPass {
 public:
  RenderPass(Device *device, VkFormat color_format);
  RenderPass(Device *device, VkFormat color_format, VkFormat depth_format);
  RenderPass(Device *device,
             const helper::AttachmentParameters &attachment_parameters);
  ~RenderPass();

 private:
  void ConstructorCommon(
      Device *device,
      const helper::AttachmentParameters &attachment_parameters);
  GRASSLAND_VULKAN_HANDLE(VkRenderPass)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
