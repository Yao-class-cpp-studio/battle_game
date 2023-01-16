#pragma once
#include <grassland/vulkan/framework/core_settings.h>
#include <grassland/vulkan/vulkan.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"

namespace grassland::vulkan::framework {
class TextureImage;
class Core {
 public:
  explicit Core(const CoreSettings &core_settings);
  ~Core();
  [[nodiscard]] const CoreSettings &GetCoreSettings() const;
  [[nodiscard]] Instance *GetInstance() const;
  [[nodiscard]] PhysicalDevice *GetPhysicalDevice() const;
  [[nodiscard]] Device *GetDevice() const;
  [[nodiscard]] Surface *GetSurface() const;
  [[nodiscard]] Swapchain *GetSwapchain() const;
  [[nodiscard]] CommandPool *GetCommandPool() const;
  [[nodiscard]] GLFWwindow *GetWindow() const;
  [[nodiscard]] CommandBuffer *GetCommandBuffer(int frame_index) const;
  [[nodiscard]] CommandBuffer *GetCommandBuffer() const;
  [[nodiscard]] int GetCurrentFrameIndex() const;
  [[nodiscard]] int GetCurrentImageIndex() const;
  [[nodiscard]] int GetWindowWidth() const;
  [[nodiscard]] int GetWindowHeight() const;
  [[nodiscard]] int GetFramebufferWidth() const;
  [[nodiscard]] int GetFramebufferHeight() const;

  void BeginCommandRecord();
  void EndCommandRecordAndSubmit();
  void TemporalSubmit();

  void Output(TextureImage *texture_image);

  void SetFrameSizeCallback(
      const std::function<void(int width, int height)> &window_size_callback);
  void SetCursorPosCallback(
      const std::function<void(double xpos, double ypos)> &cursor_pos_callback);
  void SetMouseButtonCallback(
      const std::function<void(int button, int action, int mods)>
          &mouse_button_callback);
  void SetKeyCallback(
      const std::function<void(int key, int scancode, int action, int mods)>
          &key_callback);
  void SetDropCallback(
      const std::function<void(int path_count, const char **paths)>
          &drop_callback);

  void ImGuiInit(TextureImage *render_texture,
                 const char *font_file_path = nullptr,
                 float font_size = 13.0f);
  void ImGuiRender();

 private:
  static void GLFWFrameSizeFunc(GLFWwindow *window, int width, int height);
  static void GLFWCursorPosFunc(GLFWwindow *window, double xpos, double ypos);
  static void GLFWMouseButtonFunc(GLFWwindow *window,
                                  int button,
                                  int action,
                                  int mods);
  static void GLFWKeyFunc(GLFWwindow *window,
                          int key,
                          int scancode,
                          int action,
                          int mods);
  static void GLFWDropFunc(GLFWwindow *window,
                           int path_count,
                           const char **paths);

  CoreSettings core_settings_;

  std::unique_ptr<Instance> instance_;
  std::unique_ptr<PhysicalDevice> physical_device_;
  std::unique_ptr<Device> device_;

  GLFWwindow *window_;
  std::unique_ptr<Surface> surface_;
  std::unique_ptr<Swapchain> swapchain_;

  std::unique_ptr<CommandPool> command_pool_;
  std::vector<std::unique_ptr<CommandBuffer>> command_buffers_;

  std::vector<std::unique_ptr<Fence>> in_flight_fences_;
  std::vector<std::unique_ptr<Semaphore>> image_available_semaphores_;
  std::vector<std::unique_ptr<Semaphore>> render_finish_semaphores_;

  int frame_index_{0};
  uint32_t current_image_index{0};

  std::vector<std::function<void(int width, int height)>>
      custom_window_size_function_;
  std::vector<std::function<void(double xpos, double ypos)>>
      custom_cursor_pos_function_;
  std::vector<std::function<void(int button, int action, int mods)>>
      custom_mouse_button_function_;
  std::vector<std::function<void(int key, int scancode, int action, int mods)>>
      custom_key_functions_;
  std::vector<std::function<void(int, const char **)>> custom_drop_functions_;

  VkDescriptorPool imgui_pool_{};
  TextureImage *imgui_render_target_{nullptr};
  std::unique_ptr<RenderPass> imgui_render_pass_;
  std::unique_ptr<Framebuffer> imgui_framebuffer_;
};
}  // namespace grassland::vulkan::framework
