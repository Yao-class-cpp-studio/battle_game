#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class PoisonousBullet : public Bullet {
 public:
  PoisonousBullet(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~PoisonousBullet() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float damage_rate{0.4};
  bool using_current_health{1}; // if 0: using max health
  bool is_round_down{1}; // if 0: for health count round up when hitted
};
}  // namespace battle_game::bullet