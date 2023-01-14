#pragma once
#include "battle_game/core/bullet.h"
//1

namespace battle_game::bullet {
class DisBall : public Bullet {
 public:
  DisBall(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~DisBall() override;
  void Render() override;
  void Update() override;
  int DisBall::RandomInt(int x,int y);

 private:
  glm::vec2 velocity_{};
};
}  // namespace battle_game::bullet
