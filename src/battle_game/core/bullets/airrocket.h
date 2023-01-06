#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Rocket : public Bullet {
 public:
  Rocket(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale);
  void Render() override;
  void Update() override;

 private:
  uint32_t lifetime_;
  bool hit_{false};
};
}  // namespace battle_game::bullet
