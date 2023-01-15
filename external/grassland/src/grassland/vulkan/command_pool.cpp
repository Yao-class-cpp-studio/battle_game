#include <grassland/util/logging.h>
#include <grassland/vulkan/command_pool.h>

namespace grassland::vulkan {

CommandPool::CommandPool(Device *device) : handle_{} {
  device_ = device;
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex =
      device_->GetPhysicalDevice()->GraphicsFamilyIndex();

  if (vkCreateCommandPool(device_->GetHandle(), &poolInfo, nullptr, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create command pool!");
  }
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(device_->GetHandle(), GetHandle(), nullptr);
}

}  // namespace grassland::vulkan
