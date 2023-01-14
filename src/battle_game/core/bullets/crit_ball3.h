#pragma once
#include "battle_game/core/bullet.h"
//1

namespace battle_game::bullet {
class CritBall3 : public Bullet {
 public:
  CritBall3(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~CritBall3() override;
  void Render() override;
  void Update() override;
  int CritBall3::RandomInt(int x,int y);

 private:
  glm::vec2 velocity_{};
};
}  // namespace battle_game::bullet
