#pragma once
#include "battle_game/core/bullet.h"
namespace battle_game::bullet {
class Tracking : public Bullet {
 public:
  Tracking(GameCore *core,
           uint32_t id,
           uint32_t unit_id,
           uint32_t player_id,
           glm::vec2 position,
           float rotation,
           float damage_scale,
           glm::vec2 velocity);
  ~Tracking() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float rotation_;
  uint32_t tracking_duration_{0};
  bool is_second = false;
  bool is_third = false;
  uint32_t whole_timer_ = 0;
  uint32_t partial_timer_ = 0;
};
}  // namespace battle_game::bullet
