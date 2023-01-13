#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Tank : public Unit {
 public:
  Tank(GameCore *game_core, uint32_t id, uint32_t player_id, float max_health=100.0f, float health=1.0f, float attack=1.0f, float defence=.0f, float speed=1.0f);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
};
}  // namespace battle_game::unit
