#pragma once
#include "battle_game/core/bullets/cannon_ball.h"

namespace battle_game::bullet {
class AccumulatorBullet : public CannonBall {
 public:
  AccumulatorBullet(GameCore *core,
                    uint32_t id,
                    uint32_t unit_id,
                    uint32_t player_id,
                    glm::vec2 position,
                    float rotation,
                    float damage_scale,
                    glm::vec2 velocity);
  void Render() override;

 private:
  glm::vec2 velocity_{};
};
}  // namespace battle_game::bullet
