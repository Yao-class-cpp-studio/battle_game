
#include "grassland/grassland.h"

int main() {
  grassland::vulkan::framework::CoreSettings core_settings;
  std::unique_ptr<grassland::vulkan::framework::Core> core =
      std::make_unique<grassland::vulkan::framework::Core>(core_settings);

  auto paint_buffer =
      std::make_unique<grassland::vulkan::framework::TextureImage>(
          core.get(), core->GetFramebufferWidth(), core->GetFramebufferHeight(),
          VK_FORMAT_B8G8R8A8_UNORM,
          VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  core->SetFrameSizeCallback([&paint_buffer](int width, int height) {
    paint_buffer->Resize(width, height);
  });
  core->ImGuiInit(paint_buffer.get(), nullptr, 13.0f);

  glm::vec4 color{1.0f};

  while (!glfwWindowShouldClose(core->GetWindow())) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();

    core->BeginCommandRecord();
    paint_buffer->ClearColor({0.6, 0.7, 0.8, 1.0});
    core->ImGuiRender();
    core->Output(paint_buffer.get());
    core->EndCommandRecordAndSubmit();
    glfwPollEvents();
  }

  core->GetDevice()->WaitIdle();
  paint_buffer.reset();
}
