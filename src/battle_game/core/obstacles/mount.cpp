#include "battle_game/core/obstacles/mount.h"

#include "battle_game/core/game_core.h"

namespace battle_game::obstacle {

Mount::Mount(GameCore *game_core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

bool Mount::IsBlocked(glm::vec2 p) const {
  auto pl = WorldToLocal(p);
  if (pl.x <= scale_.x && pl.x >= -scale_.x && pl.y <= scale_.y &&
      pl.y >= -scale_.y) {
    for (auto &bullet : game_core_->GetBullets()) {
      if (bullet.second->GetPosition() == p) {
        return false;
      }
    }
    return true;
  }
  return false;
}

/*
bool Mount::IsBlocked(glm::vec2 p) const {
  auto pl = WorldToLocal(p);
  if (pl.x <= scale_.x && pl.x >= -scale_.x && pl.y <= scale_.y &&
      pl.y >= -scale_.y) {
    for (auto &unit : game_core_->GetUnits()) {
      if (unit.second->GetPosition() == p) {
        unit.second->SetDamageScale(1.2);
      }
    }
  }
  return false;
}*/

void Mount::Render() {
  battle_game::SetColor(glm::vec4{0.5f, 0.5f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
