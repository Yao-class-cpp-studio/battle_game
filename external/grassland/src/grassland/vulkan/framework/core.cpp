#include <grassland/util/util.h>
#include <grassland/vulkan/framework/core.h>
#include <grassland/vulkan/framework/texture_image.h>

namespace grassland::vulkan::framework {

Core::Core(const CoreSettings &core_settings) {
  core_settings_ = core_settings;
  if (core_settings_.has_window) {
    if (!glfwInit()) {
      LAND_ERROR("[Vulkan] GLFW initialization failed.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(int(core_settings_.window_width),
                               int(core_settings_.window_height),
                               core_settings_.window_title, nullptr, nullptr);
    if (!window_) {
      LAND_ERROR("[Vulkan] GLFW create window failed.");
    }

    int actual_width, actual_height;
    glfwGetFramebufferSize(window_, &actual_width, &actual_height);
    core_settings_.window_width = actual_width;
    core_settings_.window_height = actual_height;

    glfwSetFramebufferSizeCallback(
        window_, ::grassland::vulkan::framework::Core::GLFWFrameSizeFunc);
    glfwSetCursorPosCallback(
        window_, ::grassland::vulkan::framework::Core::GLFWCursorPosFunc);
    glfwSetMouseButtonCallback(
        window_, ::grassland::vulkan::framework::Core::GLFWMouseButtonFunc);
    glfwSetKeyCallback(window_,
                       ::grassland::vulkan::framework::Core::GLFWKeyFunc);
    glfwSetDropCallback(window_,
                        ::grassland::vulkan::framework::Core::GLFWDropFunc);
    glfwSetWindowUserPointer(window_, this);
  }

  instance_ = std::make_unique<Instance>(core_settings_.has_window,
                                         core_settings_.validation_layer);

  if (core_settings_.has_window) {
    surface_ = std::make_unique<Surface>(instance_.get(), window_);
  }

  physical_device_ = std::make_unique<PhysicalDevice>(PickPhysicalDevice(
      instance_.get(), [&core_settings](PhysicalDevice physical_device) {
        if (core_settings.raytracing_pipeline_required &&
            !physical_device.HasRayTracingPipeline()) {
          return -1;
        }
        int score = 0;
        if (core_settings.has_window &&
            !physical_device.HasPresentationSupport()) {
          return -1;
        }
        score += int(physical_device.DeviceMemorySize() >> 20);
        if (physical_device.IsDiscreteGPU()) {
          score *= 2;
        }
        return score;
      }));

  physical_device_->PrintDeviceProperties();

  if (!physical_device_->GetHandle()) {
    LAND_ERROR("[Vulkan] failed to find available device.");
  }

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR
      physicalDeviceRayTracingPipelineFeatures{};
  physicalDeviceRayTracingPipelineFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
  physicalDeviceRayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
  VkPhysicalDeviceAccelerationStructureFeaturesKHR
      physicalDeviceAccelerationStructureFeatures{};
  physicalDeviceAccelerationStructureFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
  physicalDeviceAccelerationStructureFeatures.accelerationStructure = VK_TRUE;
  physicalDeviceAccelerationStructureFeatures.pNext =
      &physicalDeviceRayTracingPipelineFeatures;
  VkPhysicalDeviceBufferDeviceAddressFeaturesEXT
      physicalDeviceBufferDeviceAddressFeatures{};
  physicalDeviceBufferDeviceAddressFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT;
  physicalDeviceBufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;
  physicalDeviceBufferDeviceAddressFeatures.pNext =
      &physicalDeviceAccelerationStructureFeatures;

  device_ = std::make_unique<Device>(
      physical_device_.get(),
      core_settings_.has_window ? surface_.get() : nullptr,
      core_settings_.raytracing_pipeline_required
          ? std::vector<
                const char *>{VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                              VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                              VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                              VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME}
          : std::vector<const char *>{},
      core_settings_.validation_layer,
      core_settings_.raytracing_pipeline_required
          ? &physicalDeviceBufferDeviceAddressFeatures
          : nullptr);

  if (core_settings_.has_window) {
    swapchain_ = std::make_unique<Swapchain>(device_.get(), window_);
  }

  command_pool_ = std::make_unique<CommandPool>(device_.get());
  command_buffers_.clear();
  for (int i = 0; i < core_settings_.frames_in_flight; i++) {
    command_buffers_.push_back(
        std::make_unique<CommandBuffer>(command_pool_.get()));
  }

  for (int i = 0; i < core_settings_.frames_in_flight; i++) {
    in_flight_fences_.push_back(std::make_unique<Fence>(device_.get()));
    image_available_semaphores_.push_back(
        std::make_unique<Semaphore>(device_.get()));
    render_finish_semaphores_.push_back(
        std::make_unique<Semaphore>(device_.get()));
  }
}

Core::~Core() {
  device_->WaitIdle();
  if (imgui_render_target_) {
    imgui_framebuffer_.reset();
    imgui_render_pass_.reset();

    vkDestroyDescriptorPool(device_->GetHandle(), imgui_pool_, nullptr);
    ImGui_ImplVulkan_Shutdown();
  }
  render_finish_semaphores_.clear();
  image_available_semaphores_.clear();
  in_flight_fences_.clear();
  command_buffers_.clear();
  command_pool_.reset();
  swapchain_.reset();
  device_.reset();
  physical_device_.reset();
  if (core_settings_.has_window) {
    surface_.reset();
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
  instance_.reset();
}

const CoreSettings &Core::GetCoreSettings() const {
  return core_settings_;
}

Instance *Core::GetInstance() const {
  return instance_.get();
}

PhysicalDevice *Core::GetPhysicalDevice() const {
  return physical_device_.get();
}

Device *Core::GetDevice() const {
  return device_.get();
}

Surface *Core::GetSurface() const {
  return surface_.get();
}

Swapchain *Core::GetSwapchain() const {
  return swapchain_.get();
}

CommandPool *Core::GetCommandPool() const {
  return command_pool_.get();
}

GLFWwindow *Core::GetWindow() const {
  return window_;
}

CommandBuffer *Core::GetCommandBuffer(int frame_index) const {
  return command_buffers_[frame_index].get();
}

CommandBuffer *Core::GetCommandBuffer() const {
  return GetCommandBuffer(frame_index_);
}

int Core::GetCurrentFrameIndex() const {
  return frame_index_;
}

void Core::BeginCommandRecord() {
  frame_index_ =
      int(uint32_t((frame_index_ + 1u)) % core_settings_.frames_in_flight);
  vkWaitForFences(device_->GetHandle(), 1,
                  &in_flight_fences_[frame_index_]->GetHandle(), VK_TRUE,
                  UINT64_MAX);

  if (core_settings_.has_window) {
    vkAcquireNextImageKHR(
        device_->GetHandle(), swapchain_->GetHandle(), UINT64_MAX,
        image_available_semaphores_[frame_index_]->GetHandle(), VK_NULL_HANDLE,
        &current_image_index);
  }

  vkResetFences(device_->GetHandle(), 1,
                &in_flight_fences_[frame_index_]->GetHandle());

  vkResetCommandBuffer(command_buffers_[frame_index_]->GetHandle(),
                       /*VkCommandBufferResetFlagBits*/ 0);

  vulkan::helper::CommandBegin(command_buffers_[frame_index_]->GetHandle());
}

void Core::TemporalSubmit() {
  vulkan::helper::CommandEnd(command_buffers_[frame_index_]->GetHandle());
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      image_available_semaphores_[frame_index_]->GetHandle()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[] = {
      render_finish_semaphores_[frame_index_]->GetHandle()};

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffers_[frame_index_]->GetHandle();
  submitInfo.waitSemaphoreCount = 0;
  submitInfo.pWaitSemaphores = nullptr;
  submitInfo.pWaitDstStageMask = nullptr;
  submitInfo.signalSemaphoreCount = 0;
  submitInfo.pSignalSemaphores = nullptr;

  if (vkQueueSubmit(device_->GetGraphicsQueue()->GetHandle(), 1, &submitInfo,
                    in_flight_fences_[frame_index_]->GetHandle()) !=
      VK_SUCCESS) {
    LAND_ERROR("failed to submit draw command buffer!");
  }
  vkWaitForFences(device_->GetHandle(), 1,
                  &in_flight_fences_[frame_index_]->GetHandle(), VK_TRUE,
                  UINT64_MAX);
  vkResetFences(device_->GetHandle(), 1,
                &in_flight_fences_[frame_index_]->GetHandle());
  vkResetCommandBuffer(command_buffers_[frame_index_]->GetHandle(),
                       /*VkCommandBufferResetFlagBits*/ 0);
  vulkan::helper::CommandBegin(command_buffers_[frame_index_]->GetHandle());
}

void Core::EndCommandRecordAndSubmit() {
  vulkan::helper::CommandEnd(command_buffers_[frame_index_]->GetHandle());

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      image_available_semaphores_[frame_index_]->GetHandle()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[] = {
      render_finish_semaphores_[frame_index_]->GetHandle()};

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffers_[frame_index_]->GetHandle();

  if (core_settings_.has_window) {
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
  } else {
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
  }

  if (vkQueueSubmit(device_->GetGraphicsQueue()->GetHandle(), 1, &submitInfo,
                    in_flight_fences_[frame_index_]->GetHandle()) !=
      VK_SUCCESS) {
    LAND_ERROR("failed to submit draw command buffer!");
  }

  if (core_settings_.has_window) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain_->GetHandle()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &current_image_index;

    auto result = vkQueuePresentKHR(device_->GetPresentQueue()->GetHandle(),
                                    &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      glfwPollEvents();
    }
  }
}

void Core::Output(TextureImage *texture_image) {
  TransitImageLayout(GetCommandBuffer()->GetHandle(),
                     swapchain_->GetImage(current_image_index),
                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_NONE,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                     VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
  TransitImageLayout(
      GetCommandBuffer()->GetHandle(), texture_image->GetImage()->GetHandle(),
      VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
      VK_ACCESS_NONE, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT,
      VK_IMAGE_ASPECT_COLOR_BIT);
  VkImageCopy imageCopy{};
  imageCopy.srcOffset = VkOffset3D{0, 0, 0};
  imageCopy.dstOffset = VkOffset3D{0, 0, 0};

  imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageCopy.dstSubresource.mipLevel = 0;
  imageCopy.dstSubresource.baseArrayLayer = 0;
  imageCopy.dstSubresource.layerCount = 1;
  imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageCopy.srcSubresource.mipLevel = 0;
  imageCopy.srcSubresource.baseArrayLayer = 0;
  imageCopy.srcSubresource.layerCount = 1;
  imageCopy.extent = {swapchain_->GetExtent().width,
                      swapchain_->GetExtent().height, 1};
  vkCmdCopyImage(GetCommandBuffer()->GetHandle(),
                 texture_image->GetImage()->GetHandle(),
                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 swapchain_->GetImage(current_image_index),
                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
  TransitImageLayout(
      GetCommandBuffer()->GetHandle(),
      swapchain_->GetImage(current_image_index),
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE,
      VK_IMAGE_ASPECT_COLOR_BIT);
  TransitImageLayout(
      GetCommandBuffer()->GetHandle(), texture_image->GetImage()->GetHandle(),
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE,
      VK_IMAGE_ASPECT_COLOR_BIT);
}

void Core::SetFrameSizeCallback(
    const std::function<void(int, int)> &window_size_callback) {
  custom_window_size_function_.push_back(window_size_callback);
}
void Core::SetCursorPosCallback(
    const std::function<void(double, double)> &cursor_pos_callback) {
  custom_cursor_pos_function_.push_back(cursor_pos_callback);
}
void Core::SetMouseButtonCallback(
    const std::function<void(int, int, int)> &mouse_button_callback) {
  custom_mouse_button_function_.push_back(mouse_button_callback);
}
void Core::SetKeyCallback(
    const std::function<void(int, int, int, int)> &key_callback) {
  custom_key_functions_.push_back(key_callback);
}

void Core::SetDropCallback(
    const std::function<void(int, const char **)> &drop_callback) {
  custom_drop_functions_.push_back(drop_callback);
}

void Core::GLFWFrameSizeFunc(GLFWwindow *window, int width, int height) {
  auto core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));
  core->core_settings_.window_width = width;
  core->core_settings_.window_height = height;
  core->swapchain_.reset();
  core->swapchain_ = std::make_unique<Swapchain>(core->GetDevice(), window);

  if (!core->custom_window_size_function_.empty()) {
    for (auto &func : core->custom_window_size_function_) {
      func(width, height);
    }
  }
}

void Core::GLFWCursorPosFunc(GLFWwindow *window, double xpos, double ypos) {
  auto core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

  if (!core->custom_cursor_pos_function_.empty()) {
    for (auto &func : core->custom_cursor_pos_function_) {
      func(xpos, ypos);
    }
  }
}

void Core::GLFWMouseButtonFunc(GLFWwindow *window,
                               int button,
                               int action,
                               int mods) {
  auto core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));
  if (!core->custom_mouse_button_function_.empty()) {
    for (auto &func : core->custom_mouse_button_function_) {
      func(button, action, mods);
    }
  }
}

void Core::GLFWKeyFunc(GLFWwindow *window,
                       int key,
                       int scancode,
                       int action,
                       int mods) {
  auto core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));
  if (!core->custom_key_functions_.empty()) {
    for (auto &func : core->custom_key_functions_) {
      func(key, scancode, action, mods);
    }
  }
}

void Core::GLFWDropFunc(GLFWwindow *window,
                        int path_count,
                        const char **paths) {
  auto core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));
  if (!core->custom_drop_functions_.empty()) {
    for (auto &func : core->custom_drop_functions_) {
      func(path_count, paths);
    }
  }
}

int Core::GetWindowWidth() const {
  int width;
  glfwGetWindowSize(window_, &width, nullptr);
  return width;
}

int Core::GetWindowHeight() const {
  int height;
  glfwGetWindowSize(window_, nullptr, &height);
  return height;
}

int Core::GetFramebufferWidth() const {
  int width;
  glfwGetFramebufferSize(window_, &width, nullptr);
  return width;
}

int Core::GetFramebufferHeight() const {
  int height;
  glfwGetFramebufferSize(window_, nullptr, &height);
  return height;
}

int Core::GetCurrentImageIndex() const {
  return current_image_index;
}

void Core::ImGuiInit(TextureImage *render_texture,
                     const char *font_file_path,
                     float font_size) {
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000;
  pool_info.poolSizeCount = std::size(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

  vkCreateDescriptorPool(device_->GetHandle(), &pool_info, nullptr,
                         &imgui_pool_);

  // 2: initialize imgui library

  // this initializes the core structures of imgui
  ImGui::CreateContext();
  ImGui::StyleColorsClassic();
  ImGui_ImplGlfw_InitForVulkan(window_, true);

  // this initializes imgui for Vulkan
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = instance_->GetHandle();
  init_info.PhysicalDevice = physical_device_->GetHandle();
  init_info.Device = device_->GetHandle();
  init_info.Queue = device_->GetGraphicsQueue()->GetHandle();
  init_info.DescriptorPool = imgui_pool_;
  init_info.MinImageCount = 3;
  init_info.ImageCount = 3;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  imgui_render_target_ = render_texture;
  imgui_render_pass_ = std::make_unique<RenderPass>(
      device_.get(), imgui_render_target_->GetImage()->GetFormat());
  imgui_framebuffer_ = std::make_unique<Framebuffer>(
      device_.get(), imgui_render_target_->GetImage()->GetWidth(),
      imgui_render_target_->GetImage()->GetHeight(), imgui_render_pass_.get(),
      imgui_render_target_->GetImageView());
  ImGui_ImplVulkan_Init(&init_info, imgui_render_pass_->GetHandle());

  SetFrameSizeCallback([this](int width, int height) {
    imgui_framebuffer_ = std::make_unique<Framebuffer>(
        device_.get(), imgui_render_target_->GetImage()->GetWidth(),
        imgui_render_target_->GetImage()->GetHeight(), imgui_render_pass_.get(),
        imgui_render_target_->GetImageView());
  });

  auto &io = ImGui::GetIO();
  if (font_file_path) {
    io.Fonts->AddFontFromFileTTF(font_file_path, font_size, nullptr,
                                 io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->Build();
  } else {
    ImFontConfig im_font_config{};
    im_font_config.SizePixels = font_size;
    io.Fonts->AddFontDefault(&im_font_config);
  }

  // execute a gpu command to upload imgui font textures
  grassland::vulkan::helper::SingleTimeCommands(
      command_pool_.get(),
      [&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

  // clear font textures from cpu data
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Core::ImGuiRender() {
  auto command_buffer = GetCommandBuffer()->GetHandle();
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = imgui_render_pass_->GetHandle();
  renderPassInfo.framebuffer = imgui_framebuffer_->GetHandle();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = imgui_framebuffer_->GetExtent();
  renderPassInfo.clearValueCount = 0;
  renderPassInfo.pClearValues = nullptr;

  vkCmdBeginRenderPass(command_buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
  vkCmdEndRenderPass(command_buffer);
}
}  // namespace grassland::vulkan::framework
