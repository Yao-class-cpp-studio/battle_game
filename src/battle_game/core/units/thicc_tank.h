#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class ThiccTank : public Unit {
 public:
  ThiccTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] float GetSpeedScale() const override;
  [[nodiscard]] float GetHealthScale() const override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  void DeathCall(uint32_t src_unit_id) override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
};
}  // namespace battle_game::unit
