#pragma once
#include <grassland/vulkan/framework/core.h>
#include <grassland/vulkan/framework/data_buffer.h>
#include <grassland/vulkan/framework/texture_image.h>
#include <grassland/vulkan/framework/uniform_bindings.h>

namespace grassland::vulkan::framework {
class RenderNode {
 public:
  explicit RenderNode(Core *core);
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
  int AddColorAttachment(
      VkFormat format,
      const VkPipelineColorBlendAttachmentState &blend_state);
  int AddColorAttachment(VkFormat format, bool blend_enable = false);
  int AddColorAttachment(
      TextureImage *color_image,
      const VkPipelineColorBlendAttachmentState &blend_state);
  int AddColorAttachment(TextureImage *color_image, bool blend_enable = false);
  void AddDepthAttachment();
  void AddDepthAttachment(TextureImage *depth_image);

  void AddShader(const char *shader_path, VkShaderStageFlagBits shader_stage);

  void VertexInput(const std::vector<VkFormat> &vertex_inputs);

  void BuildRenderNode();
  void BuildRenderNode(uint32_t width, uint32_t height);

  void UpdateDescriptorSetBinding(int binding_index);

  void Draw(DataBuffer *vertex_buffer,
            DataBuffer *index_buffer,
            int index_count,
            int first_instance_index,
            int instance_count = 1);
  void Draw(CommandBuffer *command_buffer,
            DataBuffer *vertex_buffer,
            DataBuffer *index_buffer,
            int index_count,
            int first_instance_index,
            int instance_count = 1);
  void Draw(VkCommandBuffer command_buffer,
            DataBuffer *vertex_buffer,
            DataBuffer *index_buffer,
            int index_count,
            int first_instance_index,
            int instance_count = 1);

  void BeginDraw();
  void DrawDirect(DataBuffer *vertex_buffer,
                  DataBuffer *index_buffer,
                  int index_count,
                  int first_instance_index,
                  int instance_count = 1);
  void SetViewport(const VkViewport &viewport);
  void SetViewport(float x, float y, float width, float height);
  void SetScissorRect(const VkRect2D &scissor);
  void SetScissorRect(int x, int y, int width, int height);
  void EndDraw();

  [[nodiscard]] TextureImage *GetColorAttachmentImage(
      int color_attachment_index = 0) const;
  [[nodiscard]] TextureImage *GetDepthAttachmentImage() const;

 private:
  Core *core_{nullptr};

  std::unique_ptr<DescriptorSetLayout> descriptor_set_layout_;
  std::unique_ptr<PipelineLayout> pipeline_layout_;
  std::vector<std::unique_ptr<UniformBinding>> uniform_bindings_;
  std::unique_ptr<DescriptorPool> descriptor_pool_;
  std::vector<std::unique_ptr<DescriptorSet>> descriptor_sets_;

  std::unique_ptr<RenderPass> render_pass_;
  std::unique_ptr<Framebuffer> framebuffer_;
  std::vector<std::unique_ptr<TextureImage>> internal_attachment_textures_;
  std::vector<std::pair<TextureImage *, VkPipelineColorBlendAttachmentState>>
      color_attachments_;
  TextureImage *depth_attachment_{nullptr};

  std::vector<std::unique_ptr<ShaderModule>> shader_modules_;
  helper::ShaderStages shader_stages_;

  helper::VertexInputDescriptions vertex_input_descriptions_;

  std::unique_ptr<Pipeline> graphics_pipeline_;
};
}  // namespace grassland::vulkan::framework
