#include "grassland/vulkan/framework/ray_tracing_render_node.h"

namespace grassland::vulkan::framework {

RayTracingRenderNode::RayTracingRenderNode(Core *core) : core_(core) {
}

RayTracingRenderNode::~RayTracingRenderNode() = default;

void RayTracingRenderNode::AddUniformBinding(
    DataBuffer *uniform_buffer,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingUniform>(
      uniform_buffer, access_stage_flags));
}

void RayTracingRenderNode::AddBufferBinding(
    DataBuffer *uniform_buffer,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingBuffer>(
      uniform_buffer, access_stage_flags));
}

void RayTracingRenderNode::AddUniformBinding(
    TextureImage *texture_image,
    Sampler *sampler,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingTextureSampler>(
      texture_image, sampler, access_stage_flags));
}

void RayTracingRenderNode::AddUniformBinding(
    const std::vector<std::pair<TextureImage *, Sampler *>>
        &texture_sampler_pairs,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingTextureSamplers>(
      texture_sampler_pairs, access_stage_flags));
}

void RayTracingRenderNode::AddUniformBinding(
    TextureImage *texture_image,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingStorageTexture>(
      texture_image, access_stage_flags));
}

void RayTracingRenderNode::AddUniformBinding(
    const std::vector<TextureImage *> &texture_images,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingStorageTextures>(
      texture_images, access_stage_flags));
}

void RayTracingRenderNode::AddUniformBinding(
    raytracing::TopLevelAccelerationStructure *top_level_acceleration_structure,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(
      std::make_unique<UniformBindingAccelerationStructure>(
          top_level_acceleration_structure, access_stage_flags));
}

void RayTracingRenderNode::SetShaders(
    const std::string &ray_gen_shader_path,
    const std::string &miss_shader_path,
    const std::string &closest_hit_shader_path) {
  ray_gen_shader_ = std::make_unique<ShaderModule>(core_->GetDevice(),
                                                   ray_gen_shader_path.c_str());
  miss_shader_ = std::make_unique<ShaderModule>(core_->GetDevice(),
                                                miss_shader_path.c_str());
  hit_shader_ = std::make_unique<ShaderModule>(core_->GetDevice(),
                                               closest_hit_shader_path.c_str());
}

void RayTracingRenderNode::BuildRenderNode() {
  core_->GetDevice()->WaitIdle();
  pipeline_.reset();
  pipeline_layout_.reset();
  descriptor_sets_.clear();
  descriptor_pool_.reset();
  descriptor_set_layout_.reset();

  std::vector<VkDescriptorSetLayoutBinding> layout_bindings{};
  for (int i = 0; i < uniform_bindings_.size(); i++) {
    auto &uniform_binding = uniform_bindings_[i];
    auto layout_binding = uniform_binding->GetBinding();
    layout_binding.binding = i;
    layout_bindings.push_back(layout_binding);
  }

  descriptor_set_layout_ = std::make_unique<DescriptorSetLayout>(
      core_->GetDevice(), layout_bindings);
  pipeline_layout_ = std::make_unique<PipelineLayout>(
      core_->GetDevice(), descriptor_set_layout_.get());

  descriptor_pool_ = std::make_unique<DescriptorPool>(
      core_->GetDevice(), layout_bindings,
      core_->GetCoreSettings().frames_in_flight);
  for (int desc_set_index = 0;
       desc_set_index < core_->GetCoreSettings().frames_in_flight;
       desc_set_index++) {
    descriptor_sets_.push_back(std::make_unique<DescriptorSet>(
        descriptor_set_layout_.get(), descriptor_pool_.get()));
    std::vector<VkWriteDescriptorSet> write_descriptor_sets;
    for (int i = 0; i < uniform_bindings_.size(); i++) {
      auto &uniform_binding = uniform_bindings_[i];
      auto write_descriptor =
          uniform_binding->GetWriteDescriptorSet(desc_set_index);
      write_descriptor.dstBinding = i;
      write_descriptor.dstSet = descriptor_sets_[desc_set_index]->GetHandle();
      write_descriptor_sets.push_back(write_descriptor);
    }

    vkUpdateDescriptorSets(core_->GetDevice()->GetHandle(),
                           write_descriptor_sets.size(),
                           write_descriptor_sets.data(), 0, nullptr);
  }

  pipeline_ = std::make_unique<raytracing::RayTracingPipeline>(
      core_->GetDevice(), pipeline_layout_.get(), *ray_gen_shader_,
      *miss_shader_, *hit_shader_);
  shader_binding_table_ =
      std::make_unique<raytracing::ShaderBindingTable>(pipeline_.get());
}

void RayTracingRenderNode::Draw() {
  Draw(core_->GetFramebufferWidth(), core_->GetFramebufferHeight());
}

void RayTracingRenderNode::Draw(uint32_t width, uint32_t height) {
  for (auto &uniform_binding : uniform_bindings_) {
    uniform_binding->BeforeDraw(core_->GetCommandBuffer(),
                                core_->GetCurrentFrameIndex());
  }

  auto draw_cmd_buffers = core_->GetCommandBuffer()->GetHandle();
  vkCmdBindPipeline(draw_cmd_buffers, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
                    pipeline_->GetHandle());
  vkCmdBindDescriptorSets(
      draw_cmd_buffers, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
      pipeline_layout_->GetHandle(), 0, 1,
      &descriptor_sets_[core_->GetCurrentFrameIndex()]->GetHandle(), 0, 0);

  VkPhysicalDeviceRayTracingPipelinePropertiesKHR
      ray_tracing_pipeline_properties =
          core_->GetPhysicalDevice()->GetRayTracingProperties();
  auto aligned_size = [](uint32_t value, uint32_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
  };
  const uint32_t handle_size_aligned =
      aligned_size(ray_tracing_pipeline_properties.shaderGroupHandleSize,
                   ray_tracing_pipeline_properties.shaderGroupHandleAlignment);

  VkStridedDeviceAddressRegionKHR ray_gen_shader_sbt_entry{};
  ray_gen_shader_sbt_entry.deviceAddress =
      shader_binding_table_->GetRayGenDeviceAddress();
  ray_gen_shader_sbt_entry.stride = handle_size_aligned;
  ray_gen_shader_sbt_entry.size = handle_size_aligned;

  VkStridedDeviceAddressRegionKHR miss_shader_sbt_entry{};
  miss_shader_sbt_entry.deviceAddress =
      shader_binding_table_->GetMissDeviceAddress();
  miss_shader_sbt_entry.stride = handle_size_aligned;
  miss_shader_sbt_entry.size = handle_size_aligned;

  VkStridedDeviceAddressRegionKHR hit_shader_sbt_entry{};
  hit_shader_sbt_entry.deviceAddress =
      shader_binding_table_->GetClosestHitDeviceAddress();
  hit_shader_sbt_entry.stride = handle_size_aligned;
  hit_shader_sbt_entry.size = handle_size_aligned;

  VkStridedDeviceAddressRegionKHR callable_shader_sbt_entry{};

  vkCmdTraceRaysKHR(draw_cmd_buffers, &ray_gen_shader_sbt_entry,
                    &miss_shader_sbt_entry, &hit_shader_sbt_entry,
                    &callable_shader_sbt_entry, core_->GetFramebufferWidth(),
                    core_->GetFramebufferHeight(), 1);

  for (auto &uniform_binding : uniform_bindings_) {
    uniform_binding->AfterDraw(core_->GetCommandBuffer(),
                               core_->GetCurrentFrameIndex());
  }
}

}  // namespace grassland::vulkan::framework
