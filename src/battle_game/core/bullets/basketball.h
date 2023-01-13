#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Basketball : public Bullet {
 public:
  Basketball(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~Basketball() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  uint32_t rebounding_times_left_{2};
};
}  // namespace battle_game::bullet
