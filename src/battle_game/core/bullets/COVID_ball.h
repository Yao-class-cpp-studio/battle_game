#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class CovidBall : public Bullet {
 public:
  CovidBall(GameCore *core,
            uint32_t id,
            uint32_t unit_id,
            uint32_t player_id,
            glm::vec2 position,
            float rotation,
            float damage_scale,
            glm::vec2 velocity);
  ~CovidBall() override;
  void Render() override;
  void Update() override;

 private:
  float velocity_rate_{};
  float velocity_angle_{};
  float gravity_{};
  float basic_damage_{1.0f};
};

}  // namespace battle_game::bullet
