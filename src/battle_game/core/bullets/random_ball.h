#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class RandomBall : public Bullet {
 public:
  RandomBall(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~RandomBall() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float crit_rate_{0.5f};
  float crit_radius_{3.0f};
  int crit_number_{10};
};
}  // namespace battle_game::bullet
