#include <grassland/util/logging.h>
#include <grassland/vulkan/image_view.h>

namespace grassland::vulkan {

ImageView::ImageView(Image *image)
    : ImageView(image->GetDevice(), image->GetHandle(), image->GetFormat()) {
}

ImageView::ImageView(Device *device, VkImage image, VkFormat format)
    : handle_{} {
  device_ = device;
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = image;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = format;
  VkFormatProperties properties{};
  vkGetPhysicalDeviceFormatProperties(device_->GetPhysicalDevice()->GetHandle(),
                                      format, &properties);
  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.subresourceRange.aspectMask =
      (properties.optimalTilingFeatures &
       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;
  if (vkCreateImageView(device_->GetHandle(), &createInfo, nullptr, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create image views!");
  }
}

ImageView::~ImageView() {
  vkDestroyImageView(device_->GetHandle(), GetHandle(), nullptr);
}

}  // namespace grassland::vulkan
