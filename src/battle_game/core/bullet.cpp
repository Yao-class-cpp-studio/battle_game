#include "battle_game/core/bullet.h"

namespace battle_game {
Bullet::Bullet(GameCore *core,
               uint32_t id,
               uint32_t unit_id,
               uint32_t player_id,
               glm::vec2 position,
               float rotation,
               float damage_scale)
    : Object(core, id, position, rotation),
      unit_id_(unit_id),
      player_id_(player_id),
      damage_scale_(damage_scale) {
}

Bullet::~Bullet() = default;
}  // namespace battle_game
