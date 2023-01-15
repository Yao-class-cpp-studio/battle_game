#pragma once
#include "glm/glm.hpp"
#include "grassland/vulkan/device.h"
#include "grassland/vulkan/raytracing/bottom_level_acceleration_structure.h"

namespace grassland::vulkan::raytracing {
class TopLevelAccelerationStructure {
 public:
  TopLevelAccelerationStructure(
      Device *device,
      CommandPool *command_pool,
      const std::vector<
          std::pair<BottomLevelAccelerationStructure *, glm::mat4>> &objects);
  ~TopLevelAccelerationStructure();
  void UpdateAccelerationStructure(
      CommandPool *command_pool,
      const std::vector<
          std::pair<BottomLevelAccelerationStructure *, glm::mat4>> &objects);

 private:
  GRASSLAND_VULKAN_HANDLE(VkAccelerationStructureKHR);
  GRASSLAND_VULKAN_DEVICE_PTR;
  std::unique_ptr<Buffer> buffer_;
  VkDeviceAddress device_address_;
};
}  // namespace grassland::vulkan::raytracing
