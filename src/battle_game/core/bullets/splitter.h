#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Splitter : public Bullet {
 public:
  Splitter(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity,
             uint32_t split_num = 8);
  ~Splitter() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  uint32_t split_num_{8};
};
}  // namespace battle_game::bullet