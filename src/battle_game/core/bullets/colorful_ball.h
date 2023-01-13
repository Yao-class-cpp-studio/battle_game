#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class ColorfulBall : public Bullet {
 public:
  ColorfulBall(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity);
  ~ColorfulBall() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  uint32_t count_down_;
  glm::vec4 color_{};
  void update_color(float &a);
};
}  // namespace battle_game::bullet
