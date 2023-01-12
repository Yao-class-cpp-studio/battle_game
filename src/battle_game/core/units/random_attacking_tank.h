#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class RandomAttackingTank : public Unit {
 public:
  RandomAttackingTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  
  [[nodiscard]] float GetDamageScale() const override;
  [[nodiscard]] float GetSpeedScale() const override;
  [[nodiscard]] float BasicMaxHealth() const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  float change_of_turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t move_rotation_count_down_{0};
  uint32_t turret_rotation_count_down_{0};
};
}  // namespace battle_game::unit
