#include <grassland/util/util.h>
#include <grassland/vulkan/framework/render_node.h>

namespace grassland::vulkan::framework {
RenderNode::RenderNode(Core *core) {
  core_ = core;
}

TextureImage *RenderNode::GetColorAttachmentImage(
    int color_attachment_index) const {
  return color_attachments_[color_attachment_index].first;
}
TextureImage *RenderNode::GetDepthAttachmentImage() const {
  return depth_attachment_;
}

void RenderNode::AddUniformBinding(DataBuffer *uniform_buffer,
                                   VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingUniform>(
      uniform_buffer, access_stage_flags));
}

void RenderNode::AddBufferBinding(DataBuffer *uniform_buffer,
                                  VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingBuffer>(
      uniform_buffer, access_stage_flags));
}

void RenderNode::AddUniformBinding(TextureImage *texture_image,
                                   Sampler *sampler,
                                   VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingTextureSampler>(
      texture_image, sampler, access_stage_flags));
}
void RenderNode::AddUniformBinding(
    const std::vector<std::pair<TextureImage *, Sampler *>>
        &texture_sampler_pairs,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingTextureSamplers>(
      texture_sampler_pairs, access_stage_flags));
}
void RenderNode::AddUniformBinding(TextureImage *texture_image,
                                   VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingStorageTexture>(
      texture_image, access_stage_flags));
}
void RenderNode::AddUniformBinding(
    const std::vector<TextureImage *> &texture_images,
    VkShaderStageFlags access_stage_flags) {
  uniform_bindings_.push_back(std::make_unique<UniformBindingStorageTextures>(
      texture_images, access_stage_flags));
}

void RenderNode::VertexInput(const std::vector<VkFormat> &vertex_inputs) {
  uint32_t stride = 0;
  for (auto vertex_input : vertex_inputs) {
    stride += FormatSizeInBytes(vertex_input);
  }
  vertex_input_descriptions_.AddBinding(0, stride);
  uint32_t location = 0, offset = 0;
  for (auto vertex_input : vertex_inputs) {
    vertex_input_descriptions_.AddAttribute(0, location, vertex_input, offset);
    offset += FormatSizeInBytes(vertex_input);
    location += FormatSlot(vertex_input);
  }
}

void RenderNode::AddShader(const char *shader_path,
                           VkShaderStageFlagBits shader_stage) {
  shader_modules_.push_back(
      std::make_unique<ShaderModule>(core_->GetDevice(), shader_path));
  shader_stages_.AddShaderModule(shader_modules_.rbegin()->get(), shader_stage);
}

void RenderNode::AddDepthAttachment() {
  internal_attachment_textures_.push_back(std::make_unique<TextureImage>(
      core_, 1, 1,
      helper::FindDepthFormat(core_->GetDevice()->GetPhysicalDevice()),
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
  AddDepthAttachment(internal_attachment_textures_.rbegin()->get());
}

void RenderNode::AddDepthAttachment(TextureImage *depth_image) {
  depth_attachment_ = depth_image;
}

int RenderNode::AddColorAttachment(
    VkFormat format,
    const VkPipelineColorBlendAttachmentState &blend_state) {
  internal_attachment_textures_.push_back(
      std::make_unique<TextureImage>(core_, 1, 1, format));
  return AddColorAttachment(internal_attachment_textures_.rbegin()->get(),
                            blend_state);
}

int RenderNode::AddColorAttachment(VkFormat format, bool blend_enable) {
  return AddColorAttachment(
      format, VkPipelineColorBlendAttachmentState{
                  blend_enable ? VK_TRUE : VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA,
                  VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
                  VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                  VK_BLEND_OP_ADD,
                  VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT |
                      VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT});
}

int RenderNode::AddColorAttachment(
    TextureImage *color_image,
    const VkPipelineColorBlendAttachmentState &blend_state) {
  color_attachments_.emplace_back(color_image, blend_state);
  return int(color_attachments_.size()) - 1;
}

int RenderNode::AddColorAttachment(TextureImage *color_image,
                                   bool blend_enable) {
  return AddColorAttachment(
      color_image,
      VkPipelineColorBlendAttachmentState{
          blend_enable ? VK_TRUE : VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA,
          VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
          VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
          VK_BLEND_OP_ADD,
          VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT |
              VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT});
}

void RenderNode::BuildRenderNode() {
  BuildRenderNode(core_->GetCoreSettings().window_width,
                  core_->GetCoreSettings().window_height);
}

void RenderNode::BuildRenderNode(uint32_t width, uint32_t height) {
  core_->GetDevice()->WaitIdle();
  graphics_pipeline_.reset();
  framebuffer_.reset();
  pipeline_layout_.reset();
  render_pass_.reset();
  descriptor_sets_.clear();
  descriptor_pool_.reset();
  descriptor_set_layout_.reset();

  for (auto &attachment_image : internal_attachment_textures_) {
    attachment_image->Resize(width, height);
  }

  helper::AttachmentParameters attachment_parameters{};
  std::vector<ImageView *> framebuffer_image_views{};
  if (depth_attachment_) {
    attachment_parameters.AddDepthStencilAttachment(
        depth_attachment_->GetImage()->GetFormat());
    framebuffer_image_views.push_back(depth_attachment_->GetImageView());
  }
  if (!color_attachments_.empty()) {
    for (auto &color_attachment_format : color_attachments_) {
      attachment_parameters.AddColorAttachment(
          color_attachment_format.first->GetImage()->GetFormat());
      framebuffer_image_views.push_back(
          color_attachment_format.first->GetImageView());
    }
  }
  render_pass_ =
      std::make_unique<RenderPass>(core_->GetDevice(), attachment_parameters);
  framebuffer_ = std::make_unique<Framebuffer>(core_->GetDevice(), width,
                                               height, render_pass_.get(),
                                               framebuffer_image_views);

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

  std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_state;
  for (auto &color_attachment : color_attachments_) {
    blend_attachment_state.push_back(color_attachment.second);
  }

  graphics_pipeline_ = std::make_unique<Pipeline>(
      core_->GetDevice(), render_pass_.get(), pipeline_layout_.get(),
      shader_stages_, vertex_input_descriptions_, blend_attachment_state,
      depth_attachment_);
}

void RenderNode::Draw(DataBuffer *vertex_buffer,
                      DataBuffer *index_buffer,
                      int index_count,
                      int first_instance_index,
                      int instance_count) {
  Draw(core_->GetCommandBuffer(), vertex_buffer, index_buffer, index_count,
       first_instance_index, instance_count);
}

void RenderNode::Draw(CommandBuffer *command_buffer,
                      DataBuffer *vertex_buffer,
                      DataBuffer *index_buffer,
                      int index_count,
                      int first_instance_index,
                      int instance_count) {
  Draw(command_buffer->GetHandle(), vertex_buffer, index_buffer, index_count,
       first_instance_index, instance_count);
}

void RenderNode::Draw(VkCommandBuffer command_buffer,
                      DataBuffer *vertex_buffer,
                      DataBuffer *index_buffer,
                      int index_count,
                      int first_instance_index,
                      int instance_count) {
  for (auto &uniform_binding : uniform_bindings_) {
    uniform_binding->BeforeDraw(core_->GetCommandBuffer(),
                                core_->GetCurrentFrameIndex());
  }

  if (!internal_attachment_textures_.empty()) {
    for (auto &color_texture : color_attachments_) {
      TransitImageLayout(
          command_buffer, color_texture.first->GetImage()->GetHandle(),
          VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
          VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL,
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    }
  }

  if (depth_attachment_) {
    TransitImageLayout(
        command_buffer, depth_attachment_->GetImage()->GetHandle(),
        VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT);
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass_->GetHandle();
  renderPassInfo.framebuffer = framebuffer_->GetHandle();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = framebuffer_->GetExtent();
  renderPassInfo.clearValueCount = 0;
  renderPassInfo.pClearValues = nullptr;

  vkCmdBeginRenderPass(command_buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline_->GetHandle());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)framebuffer_->GetExtent().width;
  viewport.height = (float)framebuffer_->GetExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = framebuffer_->GetExtent();
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);

  VkDeviceSize offsets = 0;
  vkCmdBindVertexBuffers(
      command_buffer, 0, 1,
      &vertex_buffer->GetBuffer(core_->GetCurrentFrameIndex())->GetHandle(),
      &offsets);
  vkCmdBindIndexBuffer(
      command_buffer,
      index_buffer->GetBuffer(core_->GetCurrentFrameIndex())->GetHandle(), 0,
      VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(
      command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline_layout_->GetHandle(), 0, 1,
      &descriptor_sets_[core_->GetCurrentFrameIndex()]->GetHandle(), 0,
      nullptr);

  vkCmdDrawIndexed(command_buffer, index_count, instance_count, 0, 0,
                   first_instance_index);
  vkCmdEndRenderPass(command_buffer);

  for (auto &uniform_binding : uniform_bindings_) {
    uniform_binding->AfterDraw(core_->GetCommandBuffer(),
                               core_->GetCurrentFrameIndex());
  }
}

void RenderNode::BeginDraw() {
  auto command_buffer = core_->GetCommandBuffer()->GetHandle();
  for (auto &uniform_binding : uniform_bindings_) {
    uniform_binding->BeforeDraw(core_->GetCommandBuffer(),
                                core_->GetCurrentFrameIndex());
  }

  if (!internal_attachment_textures_.empty()) {
    for (auto &color_texture : color_attachments_) {
      TransitImageLayout(
          command_buffer, color_texture.first->GetImage()->GetHandle(),
          VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
          VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL,
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    }
  }

  if (depth_attachment_) {
    TransitImageLayout(
        command_buffer, depth_attachment_->GetImage()->GetHandle(),
        VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_ACCESS_NONE, VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT);
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass_->GetHandle();
  renderPassInfo.framebuffer = framebuffer_->GetHandle();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = framebuffer_->GetExtent();
  renderPassInfo.clearValueCount = 0;
  renderPassInfo.pClearValues = nullptr;

  vkCmdBeginRenderPass(command_buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline_->GetHandle());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)framebuffer_->GetExtent().width;
  viewport.height = (float)framebuffer_->GetExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = framebuffer_->GetExtent();
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);

  vkCmdBindDescriptorSets(
      command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline_layout_->GetHandle(), 0, 1,
      &descriptor_sets_[core_->GetCurrentFrameIndex()]->GetHandle(), 0,
      nullptr);
}

void RenderNode::DrawDirect(DataBuffer *vertex_buffer,
                            DataBuffer *index_buffer,
                            int index_count,
                            int first_instance_index,
                            int instance_count) {
  auto command_buffer = core_->GetCommandBuffer()->GetHandle();
  VkDeviceSize offsets = 0;
  vkCmdBindVertexBuffers(
      command_buffer, 0, 1,
      &vertex_buffer->GetBuffer(core_->GetCurrentFrameIndex())->GetHandle(),
      &offsets);
  vkCmdBindIndexBuffer(
      command_buffer,
      index_buffer->GetBuffer(core_->GetCurrentFrameIndex())->GetHandle(), 0,
      VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(command_buffer, index_count, instance_count, 0, 0,
                   first_instance_index);
}

void RenderNode::EndDraw() {
  auto command_buffer = core_->GetCommandBuffer()->GetHandle();
  vkCmdEndRenderPass(command_buffer);

  for (auto &uniform_binding : uniform_bindings_) {
    uniform_binding->AfterDraw(core_->GetCommandBuffer(),
                               core_->GetCurrentFrameIndex());
  }
}

void RenderNode::SetViewport(const VkViewport &viewport) {
  auto command_buffer = core_->GetCommandBuffer()->GetHandle();
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
}

void RenderNode::SetViewport(float x, float y, float width, float height) {
  SetViewport({x, y, width, height, 0.0f, 1.0f});
}

void RenderNode::SetScissorRect(const VkRect2D &scissor) {
  auto command_buffer = core_->GetCommandBuffer()->GetHandle();
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void RenderNode::SetScissorRect(int x, int y, int width, int height) {
  SetScissorRect({{x, y}, {uint32_t(width), uint32_t(height)}});
}

void RenderNode::UpdateDescriptorSetBinding(int binding_index) {
  for (int desc_set_index = 0;
       desc_set_index < core_->GetCoreSettings().frames_in_flight;
       desc_set_index++) {
    auto &uniform_binding = uniform_bindings_[binding_index];
    auto write_descriptor =
        uniform_binding->GetWriteDescriptorSet(desc_set_index);
    write_descriptor.dstBinding = binding_index;
    write_descriptor.dstSet = descriptor_sets_[desc_set_index]->GetHandle();

    vkUpdateDescriptorSets(core_->GetDevice()->GetHandle(), 1,
                           &write_descriptor, 0, nullptr);
  }
}

}  // namespace grassland::vulkan::framework
