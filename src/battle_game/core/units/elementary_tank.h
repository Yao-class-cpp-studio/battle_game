#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class ElementaryTank : public Tank {
 public:
  ElementaryTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void Fire();
  void Accelerate();
  void RageShoot();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  bool accelerate_flag_ = false;
  bool rageshoot_flag_ = false;
  int accelerate_time_;
  int rageshoot_time_;
  int num_strong_bullets_ = 0;
};
}  // namespace battle_game::unit
