#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Missile : public Bullet {
 public:
  Missile(GameCore *core,
          uint32_t id,
          uint32_t unit_id,
          uint32_t player_id,
          glm::vec2 position,
          float rotation,
          float damage_scale,
          glm::vec2 velocity,
          float max_velocity);
  ~Missile() override;
  float GetMaxVelocity();
  void Render() override;
  void Update() override;

 private:
  virtual float CalcCost(glm::vec2 diff);
  virtual glm::vec2 CalcFix(glm::vec2 diff);
  glm::vec2 velocity_{};
  float max_velocity_;
  float resistance_{0.02};
};
}  // namespace battle_game::bullet