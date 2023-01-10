#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class CritTank : public Unit {
 public:
  CritTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void BlessingsOfTheAbyssalMoon();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t blessings_count_down_{0};
  uint32_t blessings_duration_{0};
  float crit_chance_{0.3f};
  float crit_damage_{0.8f};
};
}  // namespace battle_game::unit
