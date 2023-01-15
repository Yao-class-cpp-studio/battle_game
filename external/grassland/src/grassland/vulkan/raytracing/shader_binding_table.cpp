#include "grassland/vulkan/raytracing/shader_binding_table.h"

#include "cstring"

namespace grassland::vulkan::raytracing {
ShaderBindingTable::ShaderBindingTable(
    RayTracingPipeline *ray_tracing_pipeline) {
  auto aligned_size = [](uint32_t value, uint32_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
  };

  auto ray_tracing_pipeline_properties = ray_tracing_pipeline->GetDevice()
                                             ->GetPhysicalDevice()
                                             ->GetRayTracingProperties();
  const uint32_t handle_size =
      ray_tracing_pipeline_properties.shaderGroupHandleSize;
  const uint32_t handle_size_aligned =
      aligned_size(ray_tracing_pipeline_properties.shaderGroupHandleSize,
                   ray_tracing_pipeline_properties.shaderGroupHandleAlignment);
  const uint32_t handle_alignment =
      ray_tracing_pipeline_properties.shaderGroupHandleAlignment;
  const uint32_t group_count = 3;
  const uint32_t ray_gen_handle_size =
      aligned_size(handle_size_aligned,
                   ray_tracing_pipeline_properties.shaderGroupBaseAlignment);
  const uint32_t miss_handle_size =
      aligned_size(handle_size_aligned,
                   ray_tracing_pipeline_properties.shaderGroupBaseAlignment);
  const uint32_t hit_handle_size =
      aligned_size(handle_size_aligned,
                   ray_tracing_pipeline_properties.shaderGroupBaseAlignment);
  const uint32_t sbt_size = group_count * handle_size_aligned;
  const VkBufferUsageFlags sbt_buffer_usage_flags =
      VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR |
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  // Ray_gen
  // Create binding table buffers for each shader type
  buffer_ = std::make_unique<Buffer>(
      ray_tracing_pipeline->GetDevice(),
      ray_gen_handle_size + miss_handle_size + hit_handle_size,
      sbt_buffer_usage_flags,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  buffer_address_ = buffer_->GetDeviceAddress();
  ray_gen_offset_ = 0;
  miss_offset_ = ray_gen_handle_size;
  closest_hit_offset_ = ray_gen_handle_size + miss_handle_size;

  // Copy the pipeline's shader handles into a host buffer
  std::vector<uint8_t> shader_handle_storage(sbt_size);
  vkGetRayTracingShaderGroupHandlesKHR(
      ray_tracing_pipeline->GetDevice()->GetHandle(),
      ray_tracing_pipeline->GetHandle(), 0, group_count, sbt_size,
      shader_handle_storage.data());

  // Copy the shader handles from the host buffer to the binding tables
  auto *data = static_cast<uint8_t *>(buffer_->Map());
  std::memcpy(data + ray_gen_offset_, shader_handle_storage.data(),
              handle_size_aligned);
  std::memcpy(data + miss_offset_,
              shader_handle_storage.data() + handle_size_aligned,
              handle_size_aligned);
  std::memcpy(data + closest_hit_offset_,
              shader_handle_storage.data() + handle_size_aligned * 2,
              handle_size_aligned);
  buffer_->Unmap();
}

VkDeviceAddress ShaderBindingTable::GetRayGenDeviceAddress() const {
  return buffer_address_ + ray_gen_offset_;
}

VkDeviceAddress ShaderBindingTable::GetMissDeviceAddress() const {
  return buffer_address_ + miss_offset_;
}

VkDeviceAddress ShaderBindingTable::GetClosestHitDeviceAddress() const {
  return buffer_address_ + closest_hit_offset_;
}
}  // namespace grassland::vulkan::raytracing
