#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "grassland/grassland.h"

namespace battle_game {
using namespace grassland;
struct GlobalSettings {
  glm::mat4 world_to_camera{};
};

struct ObjectSettings {
  glm::mat4 local_to_world{};
  glm::vec4 color{};
};

struct ObjectVertex {
  glm::vec2 position{};
  glm::vec2 tex_coord{};
  glm::vec4 color{};
};

struct TextureInfo {
  float x;
  float y;
  float width;
  float height;
  int texture_id;
};

void SetGlobalCore(vulkan::framework::Core *core);
vulkan::framework::Core *GetGlobalCore();

glm::vec2 Rotate(glm::vec2 v, float angle);
}  // namespace battle_game
