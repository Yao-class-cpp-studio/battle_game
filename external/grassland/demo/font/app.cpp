#include "app.h"

#include <utility>

#include "glm/gtc/matrix_transform.hpp"
#include "grassland/util/util.h"

FontViewer::FontViewer(grassland::font::Mesh font_mesh)
    : font_mesh_(std::move(font_mesh)) {
}

void FontViewer::Run() {
  OnInit();
  while (!glfwWindowShouldClose(core_->GetWindow())) {
    OnLoop();
    glfwPollEvents();
  }
  OnClose();
}

void FontViewer::OnInit() {
  core_settings_.window_width = 1024;
  core_settings_.window_height = 1024;
  core_ = std::make_unique<grassland::vulkan::framework::Core>(core_settings_);
  uniform_buffer_ = std::make_unique<
      grassland::vulkan::framework::DynamicBuffer<UniformBufferObject>>(
      core_.get(), 1);
  render_node_ =
      std::make_unique<grassland::vulkan::framework::RenderNode>(core_.get());
  render_node_->AddBufferBinding(uniform_buffer_.get(),
                                 VK_SHADER_STAGE_VERTEX_BIT);
  render_node_->VertexInput({VK_FORMAT_R32G32_SFLOAT});
  render_node_->AddColorAttachment(VK_FORMAT_B8G8R8A8_UNORM);
  render_node_->AddShader("../shaders/font_shader.vert.spv",
                          VK_SHADER_STAGE_VERTEX_BIT);
  render_node_->AddShader("../shaders/font_shader.frag.spv",
                          VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node_->BuildRenderNode();
  vertex_buffer_ =
      std::make_unique<grassland::vulkan::framework::StaticBuffer<glm::vec2>>(
          core_.get(), font_mesh_.GetVertices().size());
  index_buffer_ =
      std::make_unique<grassland::vulkan::framework::StaticBuffer<uint32_t>>(
          core_.get(), font_mesh_.GetIndices().size());
  vertex_buffer_->Upload(font_mesh_.GetVertices().data());
  index_buffer_->Upload(font_mesh_.GetIndices().data());
  uniform_buffer_->operator[](0) = {
      glm::translate(glm::mat4{1.0f}, glm::vec3{-1.0f, 1.0f, 0.0f}) *
          glm::scale(glm::mat4{1.0f}, glm::vec3{2.0f, -2.0f, 1.0f}),
      glm::vec4{1.0f}};
  spdlog::info("Char advection: {}", font_mesh_.GetAdvection());
}

void FontViewer::OnLoop() {
  core_->BeginCommandRecord();
  render_node_->GetColorAttachmentImage(0)->ClearColor(
      {0.6f, 0.7f, 0.8f, 1.0f});
  render_node_->Draw(vertex_buffer_.get(), index_buffer_.get(),
                     index_buffer_->Size(), 0);
  core_->Output(render_node_->GetColorAttachmentImage(0));
  core_->EndCommandRecordAndSubmit();
}

void FontViewer::OnClose() {
  core_->GetDevice()->WaitIdle();
  vertex_buffer_.reset();
  index_buffer_.reset();
  render_node_.reset();
  uniform_buffer_.reset();
  core_.reset();
}
