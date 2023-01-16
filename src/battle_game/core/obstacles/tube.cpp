#include "battle_game/core/obstacles/tube.h"

#include <cstdlib>

namespace battle_game::obstacle {

Tube::Tube(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation,
           glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
  ver_ = rand() % 2;
}

bool Tube::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  if (ver_) {
    if (p.x <= scale_.x && p.x >= scale_.x - 0.5 && p.y <= scale_.y &&
        p.y >= -scale_.y) {
      return true;
    }
    if (p.x >= -scale_.x && p.x <= -scale_.x + 0.5 && p.y <= scale_.y &&
        p.y >= -scale_.y) {
      return true;
    } else {
      return false;
    }
  }
  if (!ver_) {
    if (p.y <= scale_.y && p.y >= scale_.y - 0.5 && p.x <= scale_.x &&
        p.x >= -scale_.x) {
      return true;
    }
    if (p.y >= -scale_.y && p.y <= -scale_.y + 0.5 && p.x <= scale_.x &&
        p.x >= -scale_.x) {
      return true;
    } else {
      return false;
    }
  }
}

void Tube::Render() {
  battle_game::SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
  battle_game::SetTexture("../../textures/one_way.png");
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
