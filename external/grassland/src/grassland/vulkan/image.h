#pragma once
#include <grassland/vulkan/buffer.h>
#include <grassland/vulkan/command_buffer.h>
#include <grassland/vulkan/command_pool.h>
#include <grassland/vulkan/device.h>
#include <grassland/vulkan/queue.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan {
class Image {
 public:
  /*
   * Notice: SRGB formats cannot work with VK_IMAGE_USAGE_STORAGE_BIT, use UNORM
   * format instead.
   * */
  Image(Device *device,
        uint32_t width,
        uint32_t height,
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
  ~Image();
  [[nodiscard]] VkFormat GetFormat() const;
  [[nodiscard]] uint32_t GetWidth() const;
  [[nodiscard]] uint32_t GetHeight() const;
  [[nodiscard]] VkImageUsageFlags GetUsage() const;
  [[deprecated]] void TransitImageLayout(CommandBuffer *command_buffer,
                                         VkImageLayout new_layout,
                                         VkPipelineStageFlags new_stage_flags,
                                         VkAccessFlags new_access_flags);
  void Update(CommandBuffer *command_buffer, Buffer *buffer);
  void Retrieve(CommandBuffer *command_buffer, Buffer *buffer);

 private:
  GRASSLAND_VULKAN_HANDLE(VkImage)
  GRASSLAND_VULKAN_DEVICE_PTR
  VkDeviceMemory device_memory_{};
  uint32_t width_{};
  uint32_t height_{};
  VkFormat format_{VK_FORMAT_R32G32B32A32_SFLOAT};
  VkImageUsageFlags usage_{VK_IMAGE_USAGE_SAMPLED_BIT |
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT};
};

void UploadImage(Queue *graphics_queue,
                 CommandPool *command_pool,
                 Image *image,
                 Buffer *buffer);

void DownloadImage(Queue *graphics_queue,
                   CommandPool *command_pool,
                   Image *image,
                   Buffer *buffer);

void UploadImage(CommandPool *command_pool, Image *image, Buffer *buffer);

void DownloadImage(CommandPool *command_pool, Image *image, Buffer *buffer);

void CopyImage(CommandPool *command_pool,
               Image *src_image,
               Image *dst_image,
               VkOffset2D src_offset = {0, 0},
               VkOffset2D dst_offset = {0, 0},
               VkExtent2D extent = {0xffffffffu, 0xffffffffu});

void TransitImageLayout(
    VkCommandBuffer command_buffer,
    VkImage image,
    VkImageLayout new_layout,
    VkPipelineStageFlags new_stage_flags,
    VkAccessFlags new_access_flags,
    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT);

void TransitImageLayout(
    VkCommandBuffer command_buffer,
    VkImage image,
    VkImageLayout old_layout,
    VkPipelineStageFlags old_stage_flags,
    VkAccessFlags old_access_flags,
    VkImageLayout new_layout,
    VkPipelineStageFlags new_stage_flags,
    VkAccessFlags new_access_flags,
    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT);

}  // namespace grassland::vulkan
