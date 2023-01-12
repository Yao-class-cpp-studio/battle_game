#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class CrashTank : public Unit {
 public:
  CrashTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Move(float move_speed, float rotate_angular_speed);
  void Crash();
  glm::highp_vec2 FindEnemy();
  void CrashTank::TurretRotate();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  bool is_crashed_ = false;
  bool is_crashing_ = false;
};
}  // namespace battle_game::unit