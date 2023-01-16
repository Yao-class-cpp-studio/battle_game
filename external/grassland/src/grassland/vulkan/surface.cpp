#include <grassland/util/logging.h>
#include <grassland/vulkan/surface.h>

namespace grassland::vulkan {

Surface::Surface(Instance *instance, GLFWwindow *window) : handle_{} {
  instance_ = instance;
  if (glfwCreateWindowSurface(instance->GetHandle(), window, nullptr,
                              &handle_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create window surface!");
  }
}

Surface::~Surface() {
  vkDestroySurfaceKHR(instance_->GetHandle(), handle_, nullptr);
}

}  // namespace grassland::vulkan
