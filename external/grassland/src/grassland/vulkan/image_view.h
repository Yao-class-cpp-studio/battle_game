#pragma once
#include <grassland/vulkan/image.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan {
class ImageView {
 public:
  explicit ImageView(Image *image);
  ImageView(Device *device, VkImage image, VkFormat format);
  ~ImageView();

 private:
  GRASSLAND_VULKAN_HANDLE(VkImageView)
  GRASSLAND_VULKAN_DEVICE_PTR
};
}  // namespace grassland::vulkan
