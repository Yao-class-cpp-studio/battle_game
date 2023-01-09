#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class VampireBullet : public Bullet {
 public:
  VampireBullet(GameCore *core,
                uint32_t id,
                uint32_t unit_id,
                uint32_t player_id,
                glm::vec2 position,
                float rotation,
                float damage_scale,
                glm::vec2 velocity);
  ~VampireBullet() override;
  void Render() override;
  void Update() override;
  void SetVampireRate(float vampire_rate = 0.25) {
    vampire_rate_ = vampire_rate;
  }

 private:
  glm::vec2 velocity_{};
  float vampire_rate_{0};
};
}  // namespace battle_game::bullet
