#include <grassland/util/logging.h>
#include <grassland/vulkan/fence.h>

namespace grassland::vulkan {
Fence::Fence(Device *device) : handle_{} {
  device_ = device;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  if (vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create fence!");
  }
}

Fence::~Fence() {
  vkDestroyFence(device_->GetHandle(), GetHandle(), nullptr);
}

}  // namespace grassland::vulkan
