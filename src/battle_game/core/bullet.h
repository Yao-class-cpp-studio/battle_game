#pragma once
#include "battle_game/core/object.h"
#include "battle_game/graphics/assets_manager.h"
#include "cstdint"

namespace battle_game {
class GameCore;
class Bullet : public Object {
 public:
  Bullet(GameCore *core,
         uint32_t id,
         uint32_t unit_id,
         uint32_t player_id,
         glm::vec2 position,
         float rotation,
         float damage_scale);
  ~Bullet() override;

 protected:
  uint32_t unit_id_{};
  uint32_t player_id_{};
  float damage_scale_{1.0f};
};
}  // namespace battle_game
