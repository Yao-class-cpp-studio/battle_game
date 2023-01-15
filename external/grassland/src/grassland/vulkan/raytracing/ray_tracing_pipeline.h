#pragma once
#include "grassland/vulkan/device.h"
#include "grassland/vulkan/pipeline.h"
#include "grassland/vulkan/raytracing/bottom_level_acceleration_structure.h"
#include "grassland/vulkan/raytracing/top_level_acceleration_structure.h"

namespace grassland::vulkan::raytracing {
class RayTracingPipeline {
 public:
  RayTracingPipeline(Device *device,
                     PipelineLayout *pipeline_layout,
                     const ShaderModule &ray_gen_shader,
                     const ShaderModule &miss_shader,
                     const ShaderModule &closest_hit_shader);
  ~RayTracingPipeline();

 private:
  GRASSLAND_VULKAN_HANDLE(VkPipeline);
  GRASSLAND_VULKAN_DEVICE_PTR;
};
}  // namespace grassland::vulkan::raytracing
