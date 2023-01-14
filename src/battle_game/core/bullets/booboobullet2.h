#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class BoobooBullet2 : public Bullet {
 public:
  BoobooBullet2(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~BoobooBullet2() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float rotation__ = 0.0f;
};
}  // namespace battle_game::bullet
