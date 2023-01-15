#include <grassland/util/logging.h>
#include <grassland/vulkan/buffer.h>
#include <grassland/vulkan/helper/helper.h>

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

Buffer::Buffer(Device *device,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties)
    : handle_{} {
  device_ = device;
  size_ = size;
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_->GetHandle(), &bufferInfo, nullptr, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device->GetHandle(), handle_, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = FindMemoryType(
      device_->GetPhysicalDevice(), memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_->GetHandle(), &allocInfo, nullptr,
                       &device_memory_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to allocate buffer memory!");
  }

  vkBindBufferMemory(device_->GetHandle(), handle_, device_memory_, 0);
}

Buffer::~Buffer() {
  vkDestroyBuffer(device_->GetHandle(), handle_, nullptr);
  vkFreeMemory(device_->GetHandle(), device_memory_, nullptr);
}

void *Buffer::Map(uint64_t size, uint64_t offset) {
  void *data;
  vkMapMemory(device_->GetHandle(), device_memory_, offset,
              std::min(size, size_ - offset), 0, &data);
  return data;
}

void Buffer::Unmap() {
  vkUnmapMemory(device_->GetHandle(), device_memory_);
}

VkDeviceMemory Buffer::GetDeviceMemory() const {
  return device_memory_;
}

VkDeviceSize Buffer::Size() const {
  return size_;
}

void Buffer::UploadData(Queue *graphics_queue,
                        CommandPool *command_pool,
                        const void *src_data,
                        VkDeviceSize size,
                        VkDeviceSize offset) {
  size = std::min(size, size_ - offset);
  Buffer host_buffer(device_, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  auto host_mapped_buffer = host_buffer.Map(size, offset);
  std::memcpy(host_mapped_buffer, src_data, size);
  host_buffer.Unmap();
  CopyBuffer(graphics_queue, command_pool, host_buffer.GetHandle(), GetHandle(),
             size, 0, offset);
}

void Buffer::RetrieveData(Queue *graphics_queue,
                          CommandPool *command_pool,
                          void *dst_data,
                          VkDeviceSize size,
                          VkDeviceSize offset) {
  DownloadData(graphics_queue, command_pool, dst_data, size, offset);
}

void Buffer::DownloadData(Queue *graphics_queue,
                          CommandPool *command_pool,
                          void *dst_data,
                          VkDeviceSize size,
                          VkDeviceSize offset) {
  size = std::min(size, size_ - offset);
  Buffer host_buffer(device_, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  CopyBuffer(graphics_queue, command_pool, GetHandle(), host_buffer.GetHandle(),
             size, 0, offset);

  auto host_mapped_buffer = host_buffer.Map(size, offset);
  std::memcpy(dst_data, host_mapped_buffer, size);
  host_buffer.Unmap();
}

void Buffer::UploadData(CommandPool *command_pool,
                        const void *src_data,
                        VkDeviceSize size,
                        VkDeviceSize offset) {
  UploadData(command_pool->GetDevice()->GetGraphicsQueue(), command_pool,
             src_data, size, offset);
}

void Buffer::RetrieveData(CommandPool *command_pool,
                          void *dst_data,
                          VkDeviceSize size,
                          VkDeviceSize offset) {
  DownloadData(command_pool->GetDevice()->GetGraphicsQueue(), command_pool,
               dst_data, size, offset);
}
void Buffer::DownloadData(CommandPool *command_pool,
                          void *dst_data,
                          VkDeviceSize size,
                          VkDeviceSize offset) {
  DownloadData(command_pool->GetDevice()->GetGraphicsQueue(), command_pool,
               dst_data, size, offset);
}

VkDeviceAddress Buffer::GetDeviceAddress() const {
  VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
  buffer_device_address_info.sType =
      VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  buffer_device_address_info.buffer = handle_;
  return vkGetBufferDeviceAddressKHR(device_->GetHandle(),
                                     &buffer_device_address_info);
}

void CopyBuffer(Queue *graphics_queue,
                CommandPool *command_pool,
                VkBuffer src_buffer,
                VkBuffer dst_buffer,
                VkDeviceSize size,
                VkDeviceSize src_offset,
                VkDeviceSize dst_offset) {
  CopyBuffer(command_pool, src_buffer, dst_buffer, size, src_offset,
             dst_offset);
}

void CopyBuffer(CommandPool *command_pool,
                VkBuffer src_buffer,
                VkBuffer dst_buffer,
                VkDeviceSize size,
                VkDeviceSize src_offset,
                VkDeviceSize dst_offset) {
  helper::SingleTimeCommands(command_pool, [&](VkCommandBuffer command_buffer) {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    copyRegion.srcOffset = src_offset;
    copyRegion.dstOffset = dst_offset;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copyRegion);
  });
}
}  // namespace grassland::vulkan
