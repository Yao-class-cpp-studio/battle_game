#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class T72Missile : public Bullet {
 public:
  T72Missile(GameCore *core,
       uint32_t id,
       uint32_t unit_id,
       uint32_t player_id,
       glm::vec2 position,
       float rotation,
       float damage_scale,
       glm::vec2 velocity,
       uint32_t life_time);
  ~T72Missile() override;
  void Render() override;
  void Update() override;
  bool MissileBlockedByObstacles(glm::vec2 p);
  float CalculateDistance_() {
      float x = (position_ - startingpoint_).x;
      float y = (position_ - startingpoint_).y;
      return sqrt(x * x + y * y);
  }

 private:
  glm::vec2 startingpoint_{};
  glm::vec2 velocity_{};
  uint32_t life_time_{};
  uint32_t total_time_{};
  
};
}  // namespace battle_game::bullet
