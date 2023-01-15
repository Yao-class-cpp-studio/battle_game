#include "app.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "grassland/util/logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace {

struct Vertex {
  glm::vec3 pos;
  glm::vec2 tex_coord;
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}}, {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f}},  {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},  {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f}},   {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};

const std::vector<uint16_t> indices = {
    0b000, 0b010, 0b001, 0b011, 0b001, 0b010, 0b100, 0b101, 0b110,
    0b111, 0b110, 0b101, 0b000, 0b100, 0b010, 0b110, 0b010, 0b100,
    0b001, 0b011, 0b101, 0b111, 0b101, 0b011, 0b000, 0b001, 0b100,
    0b001, 0b101, 0b100, 0b010, 0b110, 0b011, 0b111, 0b011, 0b110};

}  // namespace

App::App(int width, int height, const char *title) {
  width_ = width;
  height_ = height;
  title_ = title;
  extent_ = {uint32_t(width), uint32_t(height)};
  OnCreate();
}

App::~App() {
  OnDestroy();
}

void App::Run() {
  OnInit();
  OnLoop();
  OnClose();
}

void App::OnCreate() {
}

void App::OnInit() {
  instance_ = std::make_unique<vulkan::Instance>();
  physical_device_ = std::make_unique<vulkan::PhysicalDevice>(
      vulkan::PickPhysicalDevice(instance_.get(), [](vulkan::PhysicalDevice
                                                         physical_device) {
        int score = 0;
        if (physical_device.IsDiscreteGPU())
          score += 1000;
        spdlog::info(
            "{} {}", physical_device.DeviceName(),
            physical_device.IsDiscreteGPU() ? "(Discrete)" : "(Integrated)");
        score +=
            int(physical_device.GetProperties().limits.maxImageDimension2D);
        return score;
      }).GetHandle());
  spdlog::info("Picked device:");
  physical_device_->PrintDeviceProperties();
  device_ = std::make_unique<vulkan::Device>(physical_device_.get());

  for (int i = 0; i < kMaxFramesInFlight; i++) {
    depth_buffer_image_.push_back(std::make_unique<vulkan::Image>(
        device_.get(), width_, height_,
        vulkan::helper::FindDepthFormat(physical_device_.get()),
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
    depth_buffer_image_view_.push_back(std::make_unique<vulkan::ImageView>(
        depth_buffer_image_.rbegin()->get()));
    color_buffer_image_.push_back(std::make_unique<vulkan::Image>(
        device_.get(), width_, height_, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
    color_buffer_image_view_.push_back(std::make_unique<vulkan::ImageView>(
        color_buffer_image_.rbegin()->get()));
  }

  render_pass_ = std::make_unique<vulkan::RenderPass>(
      device_.get(), color_buffer_image_[0]->GetFormat(),
      depth_buffer_image_[0]->GetFormat());

  vulkan::helper::DescriptorSetLayoutBindings descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         1, VK_SHADER_STAGE_VERTEX_BIT);
  descriptorSetLayoutBindings.AddBinding(
      1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
      VK_SHADER_STAGE_FRAGMENT_BIT);
  descriptor_set_layout_ = std::make_unique<vulkan::DescriptorSetLayout>(
      device_.get(), descriptorSetLayoutBindings);

  pipeline_layout_ = std::make_unique<vulkan::PipelineLayout>(
      device_.get(), descriptor_set_layout_.get());
  vulkan::ShaderModule vert_shader(device_.get(),
                                   "../shaders/shader_base.vert.spv");
  vulkan::ShaderModule frag_shader(device_.get(),
                                   "../shaders/shader_base.frag.spv");
  vulkan::helper::ShaderStages shader_stages;
  shader_stages.AddShaderModule(&vert_shader, VK_SHADER_STAGE_VERTEX_BIT);
  shader_stages.AddShaderModule(&frag_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
  vulkan::helper::VertexInputDescriptions vertex_input_descriptions;
  vertex_input_descriptions.AddBinding(0, sizeof(Vertex));
  vertex_input_descriptions.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT,
                                         offsetof(Vertex, pos));
  vertex_input_descriptions.AddAttribute(0, 1, VK_FORMAT_R32G32_SFLOAT,
                                         offsetof(Vertex, tex_coord));
  graphics_pipeline_ = std::make_unique<vulkan::Pipeline>(
      device_.get(), render_pass_.get(), pipeline_layout_.get(), shader_stages,
      vertex_input_descriptions, true);
  framebuffers_.resize(kMaxFramesInFlight);
  for (int i = 0; i < kMaxFramesInFlight; i++) {
    framebuffers_[i] = std::make_unique<vulkan::Framebuffer>(
        device_.get(), width_, height_, render_pass_.get(),
        std::vector<vulkan::ImageView *>{color_buffer_image_view_[i].get(),
                                         depth_buffer_image_view_[i].get()});
  }
  descriptor_pool_ = std::make_unique<vulkan::DescriptorPool>(
      device_.get(), descriptorSetLayoutBindings, kMaxFramesInFlight);
  descriptor_sets_ = std::make_unique<vulkan::DescriptorSets>(
      device_.get(), descriptor_set_layout_.get(), descriptor_pool_.get(),
      kMaxFramesInFlight);

  command_pool_ = std::make_unique<vulkan::CommandPool>(device_.get());
  command_buffers_ = std::make_unique<vulkan::CommandBuffers>(
      command_pool_.get(), kMaxFramesInFlight);

  in_flight_fence_.resize(kMaxFramesInFlight);
  image_available_semaphores_.resize(kMaxFramesInFlight);
  render_finished_semaphores_.resize(kMaxFramesInFlight);
  for (int i = 0; i < kMaxFramesInFlight; i++) {
    in_flight_fence_[i] = std::make_unique<vulkan::Fence>(device_.get());
    image_available_semaphores_[i] =
        std::make_unique<vulkan::Semaphore>(device_.get());
    render_finished_semaphores_[i] =
        std::make_unique<vulkan::Semaphore>(device_.get());
  }

  vertex_buffer_ = std::make_unique<vulkan::Buffer>(
      device_.get(), vertices.size() * sizeof(Vertex),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  index_buffer_ = std::make_unique<vulkan::Buffer>(
      device_.get(), indices.size() * sizeof(uint16_t),
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  vertex_buffer_->UploadData(command_pool_.get(),
                             reinterpret_cast<const void *>(vertices.data()));
  index_buffer_->UploadData(command_pool_.get(),
                            reinterpret_cast<const void *>(indices.data()));

  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    uniform_buffers_.push_back(std::make_unique<vulkan::Buffer>(
        device_.get(), sizeof(UniformBufferObject),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
  }

  int x, y, c;
  auto image_data = stbi_load("../textures/xor_grid.png", &x, &y, &c, 4);
  auto image_buffer = std::make_unique<vulkan::Buffer>(
      device_.get(), x * y * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  std::memcpy(image_buffer->Map(), image_data, x * y * 4);
  image_buffer->Unmap();
  stbi_image_free(image_data);
  image_ = std::make_unique<vulkan::Image>(device_.get(), x, y,
                                           VK_FORMAT_R8G8B8A8_SRGB);
  image_view_ = std::make_unique<vulkan::ImageView>(image_.get());
  sampler_ = std::make_unique<vulkan::Sampler>(device_.get());
  vulkan::UploadImage(command_pool_.get(), image_.get(), image_buffer.get());

  for (size_t i = 0; i < kMaxFramesInFlight; i++) {
    vulkan::helper::UpdateDescriptorWrite(device_->GetHandle(),
                                          descriptor_sets_->GetHandle(i), 0,
                                          uniform_buffers_[i].get());
    vulkan::helper::UpdateDescriptorWrite(device_->GetHandle(),
                                          descriptor_sets_->GetHandle(i), 1,
                                          image_view_.get(), sampler_.get());
  }
}

void App::OnLoop() {
  OnUpdate();
  OnRender();
}

void App::OnClose() {
  device_->WaitIdle();

  sampler_.reset();
  image_view_.reset();
  image_.reset();
  vertex_buffer_.reset();
  index_buffer_.reset();

  uniform_buffers_.clear();

  for (int i = 0; i < kMaxFramesInFlight; i++) {
    in_flight_fence_[i].reset();
    image_available_semaphores_[i].reset();
    render_finished_semaphores_[i].reset();
  }
  command_buffers_.reset();
  command_pool_.reset();
  descriptor_sets_.reset();
  descriptor_pool_.reset();
  framebuffers_.clear();
  graphics_pipeline_.reset();
  pipeline_layout_.reset();
  descriptor_set_layout_.reset();
  render_pass_.reset();
  color_buffer_image_view_.clear();
  color_buffer_image_.clear();
  depth_buffer_image_view_.clear();
  depth_buffer_image_.clear();
  device_.reset();
  physical_device_.reset();
  instance_.reset();
}

void App::OnDestroy() {
}

void App::OnUpdate() {
}

void App::OnRender() {
  vkWaitForFences(device_->GetHandle(), 1,
                  &in_flight_fence_[currentFrame]->GetHandle(), VK_TRUE,
                  UINT64_MAX);

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
                                (float)width_ / (float)height_, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniform_buffers_[currentFrame]->Map(), &ubo, sizeof(ubo));
    uniform_buffers_[currentFrame]->Unmap();
  }();

  vkResetFences(device_->GetHandle(), 1,
                &in_flight_fence_[currentFrame]->GetHandle());

  vkResetCommandBuffer((*command_buffers_)[currentFrame],
                       /*VkCommandBufferResetFlagBits*/ 0);
  recordCommandBuffer((*command_buffers_)[currentFrame], currentFrame);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  submitInfo.waitSemaphoreCount = 0;
  submitInfo.pWaitSemaphores = nullptr;
  submitInfo.pWaitDstStageMask = nullptr;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &(*command_buffers_)[currentFrame];

  submitInfo.signalSemaphoreCount = 0;
  submitInfo.pSignalSemaphores = nullptr;

  if (vkQueueSubmit(device_->GetGraphicsQueue()->GetHandle(), 1, &submitInfo,
                    in_flight_fence_[currentFrame]->GetHandle()) !=
      VK_SUCCESS) {
    LAND_ERROR("failed to submit draw command buffer!")
  }

  device_->GetGraphicsQueue()->WaitIdle();
  auto rt_buffer = std::make_unique<vulkan::Buffer>(
      device_.get(), 4 * width_ * height_, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  vulkan::DownloadImage(command_pool_.get(),
                        color_buffer_image_[currentFrame].get(),
                        rt_buffer.get());
  auto *color_data = new char[4 * width_ * height_];
  std::memcpy(color_data, rt_buffer->Map(), 4 * width_ * height_);
  rt_buffer->Unmap();

  stbi_write_png((std::string(title_) + ".png").c_str(), width_, height_, 4,
                 color_data, width_ * 4);

  delete[] color_data;

  currentFrame = (currentFrame + 1) % kMaxFramesInFlight;
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer,
                              uint32_t imageIndex) {
  vulkan::helper::CommandBegin(commandBuffer);

  grassland::vulkan::TransitImageLayout(
      commandBuffer, color_buffer_image_[imageIndex]->GetHandle(),
      VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
      VK_ACCESS_NONE, VK_IMAGE_ASPECT_COLOR_BIT);
  grassland::vulkan::TransitImageLayout(
      commandBuffer, depth_buffer_image_[imageIndex]->GetHandle(),
      VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
      VK_ACCESS_NONE, VK_IMAGE_ASPECT_DEPTH_BIT);

  VkClearColorValue color{0.6f, 0.7f, 0.8f, 1.0f};
  VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  vkCmdClearColorImage(commandBuffer,
                       color_buffer_image_[imageIndex]->GetHandle(),
                       VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);
  VkClearDepthStencilValue depth_stencil{1.0f, 0};
  range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  vkCmdClearDepthStencilImage(
      commandBuffer, depth_buffer_image_[imageIndex]->GetHandle(),
      VK_IMAGE_LAYOUT_GENERAL, &depth_stencil, 1, &range);

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass_->GetHandle();
  renderPassInfo.framebuffer = framebuffers_[imageIndex]->GetHandle();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = extent_;

  renderPassInfo.clearValueCount = 0;
  renderPassInfo.pClearValues = nullptr;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphics_pipeline_->GetHandle());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)width_;
  viewport.height = (float)height_;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent_;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  VkDeviceSize offsets = 0;
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertex_buffer_->GetHandle(),
                         &offsets);
  vkCmdBindIndexBuffer(commandBuffer, index_buffer_->GetHandle(), 0,
                       VK_INDEX_TYPE_UINT16);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_layout_->GetHandle(), 0, 1,
                          &descriptor_sets_->GetHandle(currentFrame), 0,
                          nullptr);

  vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  vulkan::helper::CommandEnd(commandBuffer);
}
