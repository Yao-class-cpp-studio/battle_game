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
    : DestructibleBlock(game_core, id, position, rotation, scale),
      damage_scale_{damage_scale},
      split_num_{split_num},
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

}  // namespace battle_game::obstacle
