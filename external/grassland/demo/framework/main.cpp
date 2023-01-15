#include <grassland/util/util.h>
#include <grassland/vulkan/framework/framework.h>
#include <grassland/vulkan/vulkan.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace grassland::vulkan::framework;

namespace {

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint32_t> indices = {
    0b000, 0b010, 0b001, 0b011, 0b001, 0b010, 0b100, 0b101, 0b110,
    0b111, 0b110, 0b101, 0b000, 0b100, 0b010, 0b110, 0b010, 0b100,
    0b001, 0b011, 0b101, 0b111, 0b101, 0b011, 0b000, 0b001, 0b100,
    0b001, 0b101, 0b100, 0b010, 0b110, 0b011, 0b111, 0b011, 0b110};
}  // namespace

int main() {
  grassland::vulkan::framework::CoreSettings core_settings;
  core_settings.raytracing_pipeline_required = false;
  core_settings.window_title = "Hello, World!";
  core_settings.frames_in_flight = 1;
  Core core(core_settings);

  auto vertex_buffer =
      std::make_unique<StaticBuffer<Vertex>>(&core, vertices.size());
  vertex_buffer->Upload(vertices.data());

  auto index_buffer =
      std::make_unique<StaticBuffer<uint32_t>>(&core, indices.size());
  index_buffer->Upload(indices.data());

  auto uniform_buffer = std::make_unique<DynamicBuffer<UniformBufferObject>>(
      &core, size_t(1), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  std::unique_ptr<RenderNode> render_node = std::make_unique<RenderNode>(&core);

  std::unique_ptr<TextureImage> texture = std::make_unique<TextureImage>(
      &core, core_settings.window_width, core_settings.window_height);
  texture->ReadImage("../textures/xor_grid.png");
  std::unique_ptr<grassland::vulkan::Sampler> sampler =
      std::make_unique<grassland::vulkan::Sampler>(core.GetDevice());

  render_node->AddColorAttachment(core.GetSwapchain()->GetFormat());
  render_node->AddDepthAttachment();
  render_node->AddShader("../shaders/color_shader.vert.spv",
                         VK_SHADER_STAGE_VERTEX_BIT);
  render_node->AddShader("../shaders/color_shader.frag.spv",
                         VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node->AddUniformBinding(uniform_buffer.get(),
                                 VK_SHADER_STAGE_VERTEX_BIT);
  // render_node->AddUniformBinding(texture.get(), sampler.get(),
  // VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node->VertexInput(
      {VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT});
  render_node->BuildRenderNode();

  core.SetFrameSizeCallback([&](int width, int height) {
    render_node->BuildRenderNode(width, height);
  });

  while (!glfwWindowShouldClose(core.GetWindow())) {
    [&]() {
      static auto startTime = std::chrono::high_resolution_clock::now();

      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration<float, std::chrono::seconds::period>(
                       currentTime - startTime)
                       .count();

      UniformBufferObject ubo{};
      ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                              glm::vec3(0.0f, 0.0f, 1.0f));
      ubo.view =
          glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, 1.0f));
      ubo.proj = glm::perspective(glm::radians(45.0f),
                                  (float)core_settings.window_width /
                                      (float)core_settings.window_height,
                                  0.1f, 10.0f);
      ubo.proj[1][1] *= -1;
      uniform_buffer->operator[](0ll) = ubo;
    }();
    // LAND_INFO("{} {}", render_node->GetColorImage(0)->GetImage()->GetWidth(),
    // render_node->GetColorImage(0)->GetImage()->GetHeight());
    core.BeginCommandRecord();
    render_node->GetColorAttachmentImage(0)->ClearColor(
        {0.8f, 0.7f, 0.6f, 1.0f});
    render_node->GetDepthAttachmentImage()->ClearDepth({1.0f, 0});
    render_node->Draw(vertex_buffer.get(), index_buffer.get(),
                      index_buffer->Size(), 0, 1);
    core.Output(render_node->GetColorAttachmentImage(0));
    core.EndCommandRecordAndSubmit();

    glfwPollEvents();
  }
  core.GetDevice()->WaitIdle();
  render_node.reset();
  uniform_buffer.reset();
  vertex_buffer.reset();
  index_buffer.reset();
}
