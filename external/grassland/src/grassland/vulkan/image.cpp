#include <grassland/util/logging.h>
#include <grassland/vulkan/helper/helper.h>
#include <grassland/vulkan/image.h>

namespace grassland::vulkan {

namespace {
uint32_t FindMemoryType(PhysicalDevice *physical_device,
                        uint32_t typeFilter,
                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physical_device->GetHandle(),
                                      &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  LAND_ERROR("[Vulkan] failed to find suitable memory type!");
}
}  // namespace

Image::Image(Device *device,
             uint32_t width,
             uint32_t height,
             VkFormat format,
             VkImageUsageFlags usage,
             VkImageTiling tiling)
    : handle_{} {
  device_ = device;
  width_ = width;
  height_ = height;
  format_ = format;
  usage_ = usage;

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device_->GetHandle(), &imageInfo, nullptr, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device_->GetHandle(), handle_, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = FindMemoryType(
      device_->GetPhysicalDevice(), memRequirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (vkAllocateMemory(device_->GetHandle(), &allocInfo, nullptr,
                       &device_memory_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to allocate image memory!");
  }

  vkBindImageMemory(device_->GetHandle(), handle_, device_memory_, 0);

  auto command_pool = std::make_unique<CommandPool>(device_);
  helper::SingleTimeCommands(
      command_pool.get(), [&](VkCommandBuffer command_buffer) {
        ::grassland::vulkan::TransitImageLayout(
            command_buffer, handle_, VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE,
            format == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_ASPECT_DEPTH_BIT
                                           : VK_IMAGE_ASPECT_COLOR_BIT);
      });
}

Image::~Image() {
  vkFreeMemory(device_->GetHandle(), device_memory_, nullptr);
  vkDestroyImage(device_->GetHandle(), handle_, nullptr);
}

VkFormat Image::GetFormat() const {
  return format_;
}
uint32_t Image::GetWidth() const {
  return width_;
}
uint32_t Image::GetHeight() const {
  return height_;
}

void Image::TransitImageLayout(CommandBuffer *command_buffer,
                               VkImageLayout new_layout,
                               VkPipelineStageFlags new_stage_flags,
                               VkAccessFlags new_access_flags) {
  grassland::vulkan::TransitImageLayout(
      command_buffer->GetHandle(), GetHandle(), new_layout, new_stage_flags,
      new_access_flags, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Image::Update(CommandBuffer *command_buffer, Buffer *buffer) {
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width_, height_, 1};

  vkCmdCopyBufferToImage(command_buffer->GetHandle(), buffer->GetHandle(),
                         GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &region);
}

void Image::Retrieve(CommandBuffer *command_buffer, Buffer *buffer) {
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width_, height_, 1};

  vkCmdCopyImageToBuffer(command_buffer->GetHandle(), GetHandle(),
                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                         buffer->GetHandle(), 1, &region);
}

VkImageUsageFlags Image::GetUsage() const {
  return usage_;
}

void UploadImage(Queue *graphics_queue,
                 CommandPool *command_pool,
                 Image *image,
                 Buffer *buffer) {
  UploadImage(command_pool, image, buffer);
}

void DownloadImage(Queue *graphics_queue,
                   CommandPool *command_pool,
                   Image *image,
                   Buffer *buffer) {
  DownloadImage(command_pool, image, buffer);
}

void UploadImage(CommandPool *command_pool, Image *image, Buffer *buffer) {
  helper::SingleTimeCommands(command_pool, [&](CommandBuffer *command_buffer) {
    TransitImageLayout(command_buffer->GetHandle(), image->GetHandle(),
                       VK_IMAGE_LAYOUT_GENERAL,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_NONE,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

    image->Update(command_buffer, buffer);

    TransitImageLayout(command_buffer->GetHandle(), image->GetHandle(),
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE,
                       VK_IMAGE_ASPECT_COLOR_BIT);
  });
}

void DownloadImage(CommandPool *command_pool, Image *image, Buffer *buffer) {
  helper::SingleTimeCommands(command_pool, [&](CommandBuffer *command_buffer) {
    TransitImageLayout(command_buffer->GetHandle(), image->GetHandle(),
                       VK_IMAGE_LAYOUT_GENERAL,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_NONE,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

    image->Retrieve(command_buffer, buffer);

    TransitImageLayout(
        command_buffer->GetHandle(), image->GetHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);
  });
}

void CopyImage(CommandPool *command_pool,
               Image *src_image,
               Image *dst_image,
               VkOffset2D src_offset,
               VkOffset2D dst_offset,
               VkExtent2D extent) {
  helper::SingleTimeCommands(command_pool, [&](CommandBuffer *command_buffer) {
    TransitImageLayout(command_buffer->GetHandle(), dst_image->GetHandle(),
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    TransitImageLayout(command_buffer->GetHandle(), src_image->GetHandle(),
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    VkImageCopy imageCopy{};
    imageCopy.srcOffset = VkOffset3D{src_offset.x, src_offset.y, 0};
    imageCopy.dstOffset = VkOffset3D{dst_offset.x, dst_offset.y, 0};

    imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopy.dstSubresource.mipLevel = 0;
    imageCopy.dstSubresource.baseArrayLayer = 0;
    imageCopy.dstSubresource.layerCount = 1;
    imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopy.srcSubresource.mipLevel = 0;
    imageCopy.srcSubresource.baseArrayLayer = 0;
    imageCopy.srcSubresource.layerCount = 1;
    imageCopy.extent = {
        std::min(std::min(src_image->GetWidth() - src_offset.x,
                          dst_image->GetWidth() - dst_offset.x),
                 extent.width),
        std::min(std::min(src_image->GetHeight() - src_offset.y,
                          dst_image->GetHeight() - dst_offset.y),
                 extent.height),
        1};
    vkCmdCopyImage(command_buffer->GetHandle(), src_image->GetHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image->GetHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
  });
}

void TransitImageLayout(VkCommandBuffer command_buffer,
                        VkImage image,
                        VkImageLayout new_layout,
                        VkPipelineStageFlags new_stage_flags,
                        VkAccessFlags new_access_flags,
                        VkImageAspectFlags aspect_flags) {
  TransitImageLayout(command_buffer, image, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_NONE,
                     new_layout, new_stage_flags, new_access_flags,
                     aspect_flags);
}

void TransitImageLayout(VkCommandBuffer command_buffer,
                        VkImage image,
                        VkImageLayout old_layout,
                        VkPipelineStageFlags old_stage_flags,
                        VkAccessFlags old_access_flags,
                        VkImageLayout new_layout,
                        VkPipelineStageFlags new_stage_flags,
                        VkAccessFlags new_access_flags,
                        VkImageAspectFlags aspect_flags) {
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = aspect_flags;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  barrier.srcAccessMask = old_access_flags;
  barrier.dstAccessMask = new_access_flags;

  vkCmdPipelineBarrier(command_buffer, old_stage_flags, new_stage_flags, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
}
}  // namespace grassland::vulkan
