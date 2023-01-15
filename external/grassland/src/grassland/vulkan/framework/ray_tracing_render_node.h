#pragma once
#include "grassland/vulkan/framework/core.h"
#include "grassland/vulkan/framework/data_buffer.h"
#include "grassland/vulkan/framework/uniform_bindings.h"
#include "grassland/vulkan/raytracing/raytracing.h"

namespace grassland::vulkan::framework {
class RayTracingRenderNode {
 public:
  explicit RayTracingRenderNode(Core *core);
  ~RayTracingRenderNode();
  void AddUniformBinding(DataBuffer *uniform_buffer,
                         VkShaderStageFlags access_stage_flags);
  void AddBufferBinding(DataBuffer *uniform_buffer,
                        VkShaderStageFlags access_stage_flags);
  void AddUniformBinding(TextureImage *texture_image,
                         Sampler *sampler,
                         VkShaderStageFlags access_stage_flags);
  void AddUniformBinding(const std::vector<std::pair<TextureImage *, Sampler *>>
                             &texture_sampler_pairs,
                         VkShaderStageFlags access_stage_flags);
  void AddUniformBinding(TextureImage *texture_image,
                         VkShaderStageFlags access_stage_flags);
  void AddUniformBinding(const std::vector<TextureImage *> &texture_images,
                         VkShaderStageFlags access_stage_flags);
  void AddUniformBinding(raytracing::TopLevelAccelerationStructure
                             *top_level_acceleration_structure,
                         VkShaderStageFlags access_stage_flags);
  void SetShaders(const std::string &ray_gen_shader_path,
                  const std::string &miss_shader_path,
                  const std::string &closest_hit_shader_path);
  void BuildRenderNode();
  void Draw();
  void Draw(uint32_t width, uint32_t height);

 private:
  Core *core_{};

  std::unique_ptr<DescriptorSetLayout> descriptor_set_layout_;
  std::unique_ptr<PipelineLayout> pipeline_layout_;
  std::vector<std::unique_ptr<UniformBinding>> uniform_bindings_;
  std::unique_ptr<DescriptorPool> descriptor_pool_;
  std::vector<std::unique_ptr<DescriptorSet>> descriptor_sets_;

  std::unique_ptr<ShaderModule> ray_gen_shader_;
  std::unique_ptr<ShaderModule> miss_shader_;
  std::unique_ptr<ShaderModule> hit_shader_;

  std::unique_ptr<raytracing::RayTracingPipeline> pipeline_;
  std::unique_ptr<raytracing::ShaderBindingTable> shader_binding_table_;
};
}  // namespace grassland::vulkan::framework
