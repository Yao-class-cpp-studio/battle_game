#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class SwiftTank : public Tank {
 public:
  SwiftTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void TankMove(float move_speed, float rotate_angular_speed);
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t running_count_down_{0};
  uint32_t f_count_{1};
};
}  // namespace battle_game::unit
