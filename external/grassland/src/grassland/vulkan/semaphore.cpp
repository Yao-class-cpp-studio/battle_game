#include <grassland/util/logging.h>
#include <grassland/vulkan/semaphore.h>

namespace grassland::vulkan {

Semaphore::Semaphore(Device *device) : handle_{} {
  device_ = device;
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  if (vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr,
                        &handle_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create semaphore!");
  }
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(device_->GetHandle(), GetHandle(), nullptr);
}

}  // namespace grassland::vulkan
