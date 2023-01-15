#pragma once
#include <grassland/vulkan/framework/core.h>
#include <grassland/vulkan/image.h>
#include <grassland/vulkan/image_view.h>

namespace grassland::vulkan::framework {
class TextureImage {
 public:
  TextureImage(Core *core,
               uint32_t width,
               uint32_t height,
               VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT,
               VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                         VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                         VK_IMAGE_USAGE_STORAGE_BIT |
                                         VK_IMAGE_USAGE_SAMPLED_BIT |
                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ~TextureImage();

  void ReadImage(const char *path);
  void ReadHDRImage(const char *path);
  void WriteImage(const char *path);
  void WriteHDRImage(const char *path);
  [[nodiscard]] Core *GetCore() const;
  [[nodiscard]] Image *GetImage() const;
  [[nodiscard]] ImageView *GetImageView() const;

  void ClearColor(VkClearColorValue clear_color) const;
  void ClearDepth(VkClearDepthStencilValue clear_depth_stencil) const;

  void Resize(uint32_t width, uint32_t height);

 private:
  Core *core_;
  std::unique_ptr<Image> image_;
  std::unique_ptr<ImageView> image_view_;
};
}  // namespace grassland::vulkan::framework
