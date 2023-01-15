#pragma once
#include <grassland/vulkan/framework/data_buffer.h>

namespace grassland::vulkan::framework {
template <class Ty>
class StaticBuffer : public DataBuffer {
 public:
  StaticBuffer(Core *core,
               size_t size,
               VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  [[nodiscard]] Buffer *GetBuffer(int frame_index) const override;
  [[nodiscard]] VkDeviceSize BufferSize() const override;
  void Sync(int frame_index) override;
  void Upload(const Ty *buffer);
  void Download(Ty *buffer);
  [[nodiscard]] size_t Size() const;

 private:
  size_t size_;
  std::unique_ptr<Buffer> device_buffer_;
};

template <class Ty>
StaticBuffer<Ty>::StaticBuffer(Core *core,
                               size_t size,
                               VkBufferUsageFlags usage)
    : DataBuffer(core) {
  usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  size_ = size;
  device_buffer_ =
      std::make_unique<Buffer>(core_->GetDevice(), sizeof(Ty) * size, usage);
}

template <class Ty>
Buffer *StaticBuffer<Ty>::GetBuffer(int frame_index) const {
  return device_buffer_.get();
}

template <class Ty>
void StaticBuffer<Ty>::Upload(const Ty *buffer) {
  device_buffer_->UploadData(core_->GetCommandPool(), buffer);
}

template <class Ty>
void StaticBuffer<Ty>::Download(Ty *buffer) {
  device_buffer_->DownloadData(core_->GetCommandPool(), buffer);
}

template <class Ty>
size_t StaticBuffer<Ty>::Size() const {
  return size_;
}

template <class Ty>
void StaticBuffer<Ty>::Sync(int frame_index) {
}

template <class Ty>
VkDeviceSize StaticBuffer<Ty>::BufferSize() const {
  return size_ * sizeof(Ty);
}
}  // namespace grassland::vulkan::framework
