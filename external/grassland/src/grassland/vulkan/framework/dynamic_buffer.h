#pragma once
#include <grassland/vulkan/framework/data_buffer.h>

namespace grassland::vulkan::framework {
template <class Ty>
class DynamicBuffer : public DataBuffer {
 public:
  DynamicBuffer(Core *core,
                size_t size,
                VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  [[nodiscard]] Buffer *GetBuffer(int frame_index) const override;
  [[nodiscard]] VkDeviceSize BufferSize() const override;
  void Sync(int frame_index) override;
  [[nodiscard]] size_t Size() const;
  Ty &operator[](int64_t index);

 private:
  void RequestMapState(bool requested_map_state);
  size_t size_{0};
  Ty *mapped_ptr_{nullptr};
  std::unique_ptr<Buffer> host_buffer_;
  std::vector<std::unique_ptr<Buffer>> device_buffers_;
};

template <class Ty>
DynamicBuffer<Ty>::DynamicBuffer(Core *core,
                                 size_t size,
                                 VkBufferUsageFlags usage)
    : DataBuffer(core) {
  size_ = size;
  usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  host_buffer_ = std::make_unique<Buffer>(
      core_->GetDevice(), sizeof(Ty) * size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  for (int i = 0; i < core_->GetCoreSettings().frames_in_flight; i++) {
    device_buffers_.push_back(
        std::make_unique<Buffer>(core_->GetDevice(), sizeof(Ty) * size, usage,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
  }
}

template <class Ty>
Buffer *DynamicBuffer<Ty>::GetBuffer(int frame_index) const {
  return device_buffers_[frame_index].get();
}

template <class Ty>
void DynamicBuffer<Ty>::RequestMapState(bool requested_map_state) {
  if (requested_map_state) {
    if (!mapped_ptr_) {
      mapped_ptr_ = reinterpret_cast<Ty *>(host_buffer_->Map());
    }
  } else {
    if (mapped_ptr_) {
      mapped_ptr_ = nullptr;
      host_buffer_->Unmap();
    }
  }
}

template <class Ty>
size_t DynamicBuffer<Ty>::Size() const {
  return size_;
}

template <class Ty>
Ty &DynamicBuffer<Ty>::operator[](int64_t index) {
  this->RequestMapState(true);
  return mapped_ptr_[index];
}

template <class Ty>
void DynamicBuffer<Ty>::Sync(int frame_index) {
  this->RequestMapState(false);
  CopyBuffer(core_->GetCommandPool(), host_buffer_->GetHandle(),
             GetBuffer(frame_index)->GetHandle(), size_ * sizeof(Ty), 0, 0);
}
template <class Ty>
VkDeviceSize DynamicBuffer<Ty>::BufferSize() const {
  return size_ * sizeof(Ty);
}
}  // namespace grassland::vulkan::framework
