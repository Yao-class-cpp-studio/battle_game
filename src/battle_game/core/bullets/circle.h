#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Circle : public Bullet {
 public:
  Circle(GameCore *core,
           uint32_t id,
           uint32_t unit_id,
           uint32_t player_id,
           glm::vec2 position,
           float rotation,
           float damage_scale,
           float radius);
  ~Circle() override;
  void Render() override;
  void Update() override;

 private:
  float radius_{};
};
}  // namespace battle_game::bullet