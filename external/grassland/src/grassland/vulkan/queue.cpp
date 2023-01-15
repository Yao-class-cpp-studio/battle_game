#include <grassland/vulkan/queue.h>
#include <vulkan/vulkan.h>

namespace grassland::vulkan {

Queue::Queue(Device *device, uint32_t queue_family_index) {
  device_ = device;
  vkGetDeviceQueue(device->GetHandle(), queue_family_index, 0, &handle_);
}

void Queue::WaitIdle() {
  vkQueueWaitIdle(handle_);
}

Queue::~Queue() = default;

}  // namespace grassland::vulkan
