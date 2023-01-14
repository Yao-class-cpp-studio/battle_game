#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class ReboundingBall : public Bullet {
 public:
  ReboundingBall(GameCore *core,
                 uint32_t id,
                 uint32_t unit_id,
                 uint32_t player_id,
                 glm::vec2 position,
                 float rotation,
                 float damage_scale,
                 glm::vec2 velocity,
                 uint32_t rebounding_times = 1);
  ~ReboundingBall() override;
  void Render() override;
  void Update() override;

 private:
  float vec2_dot(glm::vec2 a, glm::vec2 b);
  glm::vec2 velocity_{};
  uint32_t rebounding_times_left_{1};
};
}  // namespace battle_game::bullet
