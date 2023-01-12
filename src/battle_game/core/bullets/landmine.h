#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Landmine : public Bullet {
 public:
  Landmine(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~Landmine() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float vDecreaseRate_{0.1};
  float timer_{0};
  float lifespan_{5};
  float visibleTime_{2};
};
}  // namespace battle_game::bullet