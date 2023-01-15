#include "rtapp.h"

#include "glm/gtc/matrix_transform.hpp"

RayTracingApp::RayTracingApp(uint32_t width, uint32_t height) {
  grassland::vulkan::framework::CoreSettings core_settings;
  core_settings.window_width = width;
  core_settings.window_height = height;
  core_settings.raytracing_pipeline_required = true;
  core_settings.validation_layer = true;
  core_ = std::make_unique<grassland::vulkan::framework::Core>(core_settings);
}

void RayTracingApp::Run() {
  OnInit();
  while (!glfwWindowShouldClose(core_->GetWindow())) {
    OnLoop();
    glfwPollEvents();
  }
  OnClose();
}

void RayTracingApp::OnInit() {
  frame_texture_ = std::make_unique<grassland::vulkan::framework::TextureImage>(
      core_.get(), core_->GetFramebufferWidth(), core_->GetFramebufferHeight(),
      VK_FORMAT_B8G8R8A8_UNORM);
  core_->SetFrameSizeCallback(
      [this](int width, int height) { frame_texture_->Resize(width, height); });

  std::vector<glm::vec3> vertices = {
      {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
  std::vector<uint32_t> indices = {0, 1, 2};
  bottom_level_acceleration_structure_ = std::make_unique<
      grassland::vulkan::raytracing::BottomLevelAccelerationStructure>(
      core_->GetDevice(), core_->GetCommandPool(), vertices, indices);
  top_level_acceleration_structure_ = std::make_unique<
      grassland::vulkan::raytracing::TopLevelAccelerationStructure>(
      core_->GetDevice(), core_->GetCommandPool(),
      std::vector<std::pair<
          grassland::vulkan::raytracing::BottomLevelAccelerationStructure *,
          glm::mat4>>{
          {bottom_level_acceleration_structure_.get(), glm::mat4{1.0f}}});
  camera_object_buffer_ = std::make_unique<
      grassland::vulkan::framework::StaticBuffer<CameraObject>>(core_.get(), 1);
  CameraObject camera_object{};
  camera_object.screen_to_camera =
      glm::inverse(glm::perspectiveLH(glm::radians(90.0f),
                                      (float)core_->GetFramebufferWidth() /
                                          (float)core_->GetFramebufferHeight(),
                                      0.1f, 10.0f));
  camera_object.camera_to_world = glm::inverse(
      glm::lookAtLH(glm::vec3{0.0f, 0.0f, -2.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                    glm::vec3{0.0f, 1.0f, 0.0f}));
  camera_object_buffer_->Upload(&camera_object);

  ray_tracing_render_node_ =
      std::make_unique<grassland::vulkan::framework::RayTracingRenderNode>(
          core_.get());
  ray_tracing_render_node_->AddUniformBinding(
      top_level_acceleration_structure_.get(), VK_SHADER_STAGE_RAYGEN_BIT_KHR);
  ray_tracing_render_node_->AddUniformBinding(frame_texture_.get(),
                                              VK_SHADER_STAGE_RAYGEN_BIT_KHR);
  ray_tracing_render_node_->AddUniformBinding(camera_object_buffer_.get(),
                                              VK_SHADER_STAGE_RAYGEN_BIT_KHR);
  ray_tracing_render_node_->SetShaders("../shaders/raytracing.rgen.spv",
                                       "../shaders/raytracing.rmiss.spv",
                                       "../shaders/raytracing.rchit.spv");
  ray_tracing_render_node_->BuildRenderNode();
}

void RayTracingApp::OnLoop() {
  OnUpdate();
  OnRender();
}

void RayTracingApp::OnClose() {
  core_->GetDevice()->WaitIdle();
  // frame_texture_.reset();
}

void RayTracingApp::OnUpdate() {
  static float theta = 0.0f;
  theta += glm::radians(0.1f);

  top_level_acceleration_structure_->UpdateAccelerationStructure(
      core_->GetCommandPool(),
      std::vector<std::pair<
          grassland::vulkan::raytracing::BottomLevelAccelerationStructure *,
          glm::mat4>>{
          {bottom_level_acceleration_structure_.get(),
           glm::rotate(glm::mat4{1.0f}, theta, glm::vec3{0.0f, 1.0f, 0.0f})}});
}

void RayTracingApp::OnRender() {
  core_->BeginCommandRecord();
  frame_texture_->ClearColor({0.6f, 0.7f, 0.8f, 1.0f});
  ray_tracing_render_node_->Draw();
  core_->Output(frame_texture_.get());
  core_->EndCommandRecordAndSubmit();
}
