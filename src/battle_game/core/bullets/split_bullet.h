#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class SplitBullet : public Bullet {
 public:
  SplitBullet(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~SplitBullet() override;
  void Render() override;
  void Update() override;
  void ResetCountDown();
 private:
  glm::vec2 velocity_{};
  uint32_t split_count_down_{0};
  uint32_t split_interval = 10;
};
}  // namespace battle_game::bullet
