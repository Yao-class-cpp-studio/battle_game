#pragma once
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

namespace battle_game {
constexpr int kKeyRange = GLFW_KEY_LAST + 1;
constexpr int kMouseButtonRange = GLFW_MOUSE_BUTTON_LAST + 1;
struct InputData {
  bool key_down[kKeyRange]{};
  bool mouse_button_down[kMouseButtonRange]{};
  bool mouse_button_clicked[kMouseButtonRange]{};
  glm::vec2 mouse_cursor_position{0.0f};
};
}  // namespace battle_game
