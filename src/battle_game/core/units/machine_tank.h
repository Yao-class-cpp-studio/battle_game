#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Machine_Tank : public Tank {
 public:
  Machine_Tank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  bool auto_fire{false};
  uint32_t auto_swithch_count_down_{0};
  bool is_stuck_by_swamp_{false};
};
}  // namespace battle_game::unit
