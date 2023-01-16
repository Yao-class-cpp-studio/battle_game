#include "grassland/vulkan/raytracing/ray_tracing_pipeline.h"

namespace grassland::vulkan::raytracing {
RayTracingPipeline::RayTracingPipeline(Device *device,
                                       PipelineLayout *pipeline_layout,
                                       const ShaderModule &ray_gen_shader,
                                       const ShaderModule &miss_shader,
                                       const ShaderModule &closest_hit_shader)
    : device_(device) {
  helper::ShaderStages shader_stages;
  std::vector<VkRayTracingShaderGroupCreateInfoKHR> shader_groups;

  // Ray generation group
  {
    shader_stages.AddShaderModule(&ray_gen_shader,
                                  VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    VkRayTracingShaderGroupCreateInfoKHR ray_gen_group_ci{};
    ray_gen_group_ci.sType =
        VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ray_gen_group_ci.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ray_gen_group_ci.generalShader = 0;
    ray_gen_group_ci.closestHitShader = VK_SHADER_UNUSED_KHR;
    ray_gen_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
    ray_gen_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
    shader_groups.push_back(ray_gen_group_ci);
  }

  // Ray miss group
  {
    shader_stages.AddShaderModule(&miss_shader, VK_SHADER_STAGE_MISS_BIT_KHR);
    VkRayTracingShaderGroupCreateInfoKHR miss_group_ci{};
    miss_group_ci.sType =
        VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    miss_group_ci.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    miss_group_ci.generalShader = 1;
    miss_group_ci.closestHitShader = VK_SHADER_UNUSED_KHR;
    miss_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
    miss_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
    shader_groups.push_back(miss_group_ci);
  }

  // Ray closest hit group
  {
    shader_stages.AddShaderModule(&closest_hit_shader,
                                  VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    VkRayTracingShaderGroupCreateInfoKHR closes_hit_group_ci{};
    closes_hit_group_ci.sType =
        VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    closes_hit_group_ci.type =
        VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    closes_hit_group_ci.generalShader = VK_SHADER_UNUSED_KHR;
    closes_hit_group_ci.closestHitShader = 2;
    closes_hit_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
    closes_hit_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
    shader_groups.push_back(closes_hit_group_ci);
  }

  /*
          Create the ray tracing pipeline
  */
  VkRayTracingPipelineCreateInfoKHR raytracing_pipeline_create_info{};
  raytracing_pipeline_create_info.sType =
      VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
  raytracing_pipeline_create_info.stageCount =
      static_cast<uint32_t>(shader_stages.Size());
  raytracing_pipeline_create_info.pStages = shader_stages.Data();
  raytracing_pipeline_create_info.groupCount =
      static_cast<uint32_t>(shader_groups.size());
  raytracing_pipeline_create_info.pGroups = shader_groups.data();
  raytracing_pipeline_create_info.maxPipelineRayRecursionDepth = 1;
  raytracing_pipeline_create_info.layout = pipeline_layout->GetHandle();
  vkCreateRayTracingPipelinesKHR(
      device_->GetHandle(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1,
      &raytracing_pipeline_create_info, nullptr, &handle_);
}

RayTracingPipeline::~RayTracingPipeline() {
  vkDestroyPipeline(device_->GetHandle(), handle_, nullptr);
}

}  // namespace grassland::vulkan::raytracing
