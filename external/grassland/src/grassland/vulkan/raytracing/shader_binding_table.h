#pragma once
#include "grassland/vulkan/buffer.h"
#include "grassland/vulkan/device.h"
#include "grassland/vulkan/raytracing/ray_tracing_pipeline.h"

namespace grassland::vulkan::raytracing {
class ShaderBindingTable {
 public:
  explicit ShaderBindingTable(RayTracingPipeline *ray_tracing_pipeline);
  [[nodiscard]] VkDeviceAddress GetRayGenDeviceAddress() const;
  [[nodiscard]] VkDeviceAddress GetMissDeviceAddress() const;
  [[nodiscard]] VkDeviceAddress GetClosestHitDeviceAddress() const;

 private:
  std::unique_ptr<Buffer> buffer_;
  VkDeviceAddress buffer_address_;
  VkDeviceAddress ray_gen_offset_;
  VkDeviceAddress miss_offset_;
  VkDeviceAddress closest_hit_offset_;
};
}  // namespace grassland::vulkan::raytracing
