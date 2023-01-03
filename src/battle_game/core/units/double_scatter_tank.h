#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class DoubleScatterTank : public Unit {
 public:
  DoubleScatterTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void DoubleScatterTankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void Scatter();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t second_scatter_count_down_{0};
  bool is_second_scatter_{false};
};
}  // namespace battle_game::unit
