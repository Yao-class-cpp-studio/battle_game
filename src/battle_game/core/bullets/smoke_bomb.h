#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class SmokeBomb : public Bullet {
 public:
  SmokeBomb(GameCore *core,
            uint32_t id,
            uint32_t unit_id,
            uint32_t player_id,
            glm::vec2 position,
            float rotation,
            float damage_scale,
            glm::vec2 target,
            float radius,
            float duration,
            float damage_duration);
  void Render() override;
  void Update() override;

 private:
  const glm::vec2 target_;
  const glm::vec2 velocity_;
  const float radius_;
  const uint32_t duration_;
  const uint32_t damage_duration_;
  uint32_t current_time_;
};
}  // namespace battle_game::bullet
