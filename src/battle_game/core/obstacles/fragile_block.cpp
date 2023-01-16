#include "battle_game/core/obstacles/fragile_block.h"

#include "battle_game/core/game_core.h"

namespace battle_game::obstacle {

FragileBlock::FragileBlock(GameCore *game_core,
                           uint32_t id,
                           glm::vec2 position,
                           float rotation,
                           glm::vec2 scale,
                           float damage_scale,
                           uint32_t split_num,
                           glm::vec2 velocity)
    : Obstacle(game_core, id, position, rotation),
      split_num_{split_num},
      damage_scale_{damage_scale},
      velocity_{velocity} {
  SetDestructible();
}

FragileBlock::~FragileBlock() {
  if (game_core_) {
    for (auto i = 0; i < split_num_; i++) {
      auto turret_rotation = i * glm::radians(360.0f) / split_num_;
      auto velocity = Rotate(velocity_, turret_rotation);
      game_core_->PushEventGenerateBullet<bullet::CannonBall>(
          0, 0, position_ + Rotate({0.0f, 1.2f}, turret_rotation),
          turret_rotation, damage_scale_, velocity);
    }
  }
}

bool FragileBlock::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

void FragileBlock::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
