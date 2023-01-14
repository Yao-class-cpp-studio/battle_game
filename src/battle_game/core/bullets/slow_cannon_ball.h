#pragma once
#include "battle_game/core/bullet.h"
#include "battle_game/core/bullets/cannon_ball.h"

namespace battle_game::bullet {
class SlowCannonBall : public Bullet {
 public:
  SlowCannonBall(GameCore *core,
                 uint32_t id,
                 uint32_t unit_id,
                 uint32_t player_id,
                 glm::vec2 position,
                 float rotation,
                 float damage_scale,
                 glm::vec2 velocity);
  void Update() override;
  void Render() override;
  ~SlowCannonBall() override;

 private:
  glm::vec2 velocity_{};
};
}  // namespace battle_game::bullet
