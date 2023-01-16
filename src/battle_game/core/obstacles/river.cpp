#include "battle_game/core/obstacles/river.h"

#include "battle_game/core/game_core.h"

namespace battle_game::obstacle {

River::River(GameCore *game_core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

bool River::IsBlocked(glm::vec2 p) const {
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

void River::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}

void River::HandleCollision(glm::vec2 &p) const {
  auto local_p = WorldToLocal(p);
  auto del1 = scale_.x - local_p.x;
  auto del2 = local_p.x + scale_.x;
  auto del3 = scale_.y - local_p.y;
  auto del4 = local_p.y + scale_.y;
  if (del1 <= del2 && del1 <= del3 && del1 <= del4) {
    local_p.x = scale_.x;
  } else if (del2 <= del1 && del2 <= del3 && del2 <= del4) {
    local_p.x = -scale_.x;
  } else if (del3 <= del1 && del3 <= del2 && del3 <= del4) {
    local_p.y = scale_.y;
  } else {
    local_p.y = -scale_.y;
  }

  p = LocalToWorld(local_p);
}
}  // namespace battle_game::obstacle
