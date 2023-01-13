#include "battle_game/graphics//util.h"

namespace battle_game {
namespace {
vulkan::framework::Core *g_core = nullptr;
}

void SetGlobalCore(vulkan::framework::Core *core) {
  g_core = core;
}

vulkan::framework::Core *GetGlobalCore() {
  return g_core;
}

glm::vec2 Rotate(glm::vec2 v, float angle) {
  return glm::vec2{
      glm::rotate(glm::mat4{1.0f}, angle, glm::vec3{0.0f, 0.0f, 1.0f}) *
      glm::vec4{v, 0.0f, 1.0f}};
}
}  // namespace battle_game
