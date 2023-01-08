#pragma once
#include "battle_game/core/unit.h"

// Sponsor : Chang'an Car
// Watch the cartoon with Chang'an Car, which is part of the plan

namespace battle_game::unit {
class ThreeBodyMan : public Unit {
 public:
  ThreeBodyMan(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void ManMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void SafetyDeclare();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t warning_line_time_{0};
  uint32_t declare_count_down_{0};
  uint32_t declare_valid_time_{0};

  glm::vec2 shoot_position_{0.0f, 0.0f};
  float shoot_rotation_{0.0f};
  float shoot_damage_scale_{0.0f};
  glm::vec2 shoot_args_{0.0f, 0.0f};
  bool shooted_{false};
};
}  // namespace battle_game::unit
